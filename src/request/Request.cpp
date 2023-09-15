#include "Request.hpp"
#include "responseCodes.hpp"
#include "Webserver.hpp"

/**
 * @brief reads a request from the socket, splits it into separate lines 
	and sends each line to the corresponding parsing function for further processing
 * 
 */
void	Request::processReq(void) 
{
	if (this->_state == READHEADERS)
		readFirstLineAndHeaders();
	if (this->_state == READBODY)
		readBody();
}

void		Request::readFirstLineAndHeaders(void) 
{
	char	socketBuffer[MAXLINE];
	std::memset(socketBuffer, 0, MAXLINE); // CHECK IF FAILED
	std::string	processingBuffer, line;
	ssize_t		bytesRead = 0;
	bool		firstLineComplete = false;

	if (this->_state == READHEADERS && (bytesRead = recv(this->_connFD, &socketBuffer, MAXLINE, 0)) > 0)
	{
		std::string	chunk(socketBuffer, bytesRead);
		processingBuffer += chunk;
		std::memset(socketBuffer, 0, MAXLINE);  // CHECK IF FAILED
		while (this->_state == READHEADERS && processingBuffer.find('\n') < std::string::npos) // if a whole (first) line is in the buffer
		{
			extractStr(processingBuffer, line, processingBuffer.find_first_of('\n'));
			if (!firstLineComplete)
				firstLineComplete = this->parseStartLine(line);
			else
			{
				this->parseFieldLine(line);
				if (processingBuffer.find("\r\n") == 0)
					this->_state = this->_method == "POST" ? READBODY : WRITE;
			}
		}
	}
	// BYTESREAD < 0? INTERNAL_SERVER_ERROR
	if (bytesRead == 0)
		this->_state = OVERWRITE;
	if (this->_state == READBODY && this->_contentLength > 0)
	{
		this->_body = processingBuffer.substr(2);
		this->_bodyLength = _body.length();
	}
	processingBuffer.clear();
}

void		Request::readBody() 
{
	char	socketBuffer[MAXLINE];
	ssize_t	bytesRead = 0;
	std::memset(socketBuffer, 0, MAXLINE);
	
	if ((bytesRead = recv(this->_connFD, &socketBuffer, MAXLINE, 0)) > 0 && this->_state != WRITE)
	{
		std::cout << "Read " << bytesRead << " bytes, total is now " << this->_bodyLength << std::endl;
		std::string	chunk(socketBuffer, bytesRead);
		// std::cout << "[***chunk IS] >" << chunk << "<" << std::endl;
		this->_body.append(chunk);
		this->_bodyLength += bytesRead;
		std::memset(socketBuffer, 0, MAXLINE); // CHECK IF FAILED
		if (this->_bodyLength == this->_contentLength || this->_body.find((this->_boundary + "--")) < std::string::npos)
			this->_state = WRITE;
	}
	if (bytesRead < 0)
	{
		// INTERNAL_SERVER_ERROR STATUSCODE
		// STATE SEND_RESPONSE OID

			// // std::string lastpart = _body.substr(_body.size() - 42);
			// // std::cerr << "[processReq] (read "  << this->_bodyLength << "/" << this->_contentLength << "), leaving loop. Last part of body is: [" << lastpart << "]" << std::endl;
			// if (this->_bodyLength == this->_contentLength || this->_body.find((this->_boundary + "--")) < std::string::npos)
			// 	this->_state = WRITE;
			// else
			// 	this->_state = READBODY;
			// std::cout << "State is now: " << this->_state << std::endl;
	}
	else if (bytesRead == 0)
		std::cerr << "[processReq] READ 0; total read body length is " << this->_bodyLength << ", contentlength is " << this->_contentLength << std::endl;
	// std::cout <<"***** WHOLE BODY IS ****" << this->_body << "****" << std::endl;
}

/**
 * @brief parses the start line of a request and saves the data in the 
	corresponding member variables
 * 
 * @param line 
 * @return true, which means that the start line has been parsed
 */
bool	Request::parseStartLine(std::string &line)
{
	size_t	end, questionMark;

	end = line.find_first_not_of(UPPERCASE);
	setMethod(line.substr(0, end));
	line.erase(0, end + 1);
	end = line.find_first_of(" ");
	setTarget(line.substr(0, end));	// WATCH OUT: TARGET CAN BE AN ABSOLUTE PATH
	line.erase(0, end + 1);
	questionMark = this->_target.find_first_of("?");
	if (questionMark < this->_target.length() - 1)
	{
		setQueryString(this->_target.substr(questionMark + 1, std::string::npos));
		this->_target.erase(questionMark, std::string::npos);
	}
	if (this->_target.find("/..") < std::string::npos)
		setStatusCode(BAD_REQUEST);
	setProtocolVersion(line.substr(0, std::string::npos)); // that's the whole line
	line.erase(0, std::string::npos);
	return (true);
}

/**
 * @brief parses each line of a request's header and saves the data in the 
	corresponding member variables
 * 
 * @param line 
 */
void	Request::parseFieldLine(std::string &line) 
{
	std::string	key, value;
	std::map<std::string,std::string>::iterator	it;
	
	key = extractKey(line);
	value = extractValue(line);
	if (key == "Host")
		setHost(value);
	else if (key == "Content-Length")
		setContentLength(value);
	else if (key == "Content-Type" && value.find("boundary=") < std::string::npos)
	{
		setBoundary(value.substr(value.find("boundary=") + 9));
		std::cout << "[parsing Field] line is >" << line << "< and boundary is now >" << this->_boundary << std::endl;
	}
	try
	{
		this->_headers.at(key) += ", " + value;
	}
	catch(const std::out_of_range & oor)
	{
		this->_headers.insert(std::pair<std::string,std::string>(key, value));
	}
	line.erase(0, std::string::npos);
}

/**
 * @brief identifies the server that should handle the request and returns a 
 * reference to it
 * 
 * @param servers the vector of existing Server instances
 * @return Server const& reference to the server that shall handle the request
 */
Server const &	Request::identifyServer(std::vector<Server> const & servers)
{
	std::vector<int>	matches;
	int					bestMatch = -1;
	int					zero = -1;
	
	findHostMatch(servers, matches, &zero);
	switch (matches.size())
	{
		case 0:
			if (zero < 0){
				this->_statusCode = INTERNAL_SERVER_ERROR;
			}
			return (servers[zero]);
		case 1:
			return (servers[matches[0]]);
		default:
			bestMatch = findServerNameMatch(servers, matches);
			return (servers[bestMatch]);
	}
}

/**
 * @brief identifies servers that match the request based on host:port combination
 * 
 * @param servers vector of existing Server instances
 * @param matches vector of integers with indexes of matching servers
 * @param zero index of the server that listens on 0.0.0.0 (if any)
 */
void	Request::findHostMatch(std::vector<Server> const & servers, \
std::vector<int> & matches, int *zero)
{
	for (size_t idx = 0; idx < servers.size(); idx++)
	{
		for (size_t i = 0; i < servers[idx].getListens().size(); i++)
		{
			std::string const & serverAddress = servers[idx].getHost(i);
			if (servers[idx].getPort(i) == this->_port && \
			(serverAddress == this->_address || this->isLocalhost(serverAddress)))
				matches.push_back(idx);
			else if (*zero < 0 && servers[idx].getHost(i) == "0.0.0.0" && \
			this->_port == servers[idx].getPort(i))
				*zero = idx;
		}
	}
	
}

/**
 * @brief identifies servers that match the request based on the hostname (in 
 * case more than one matches have been found based on host:port combination)
 * 
 * @param servers vector of existing Server instances
 * @param matches vector of integers with indexes of matching servers
 * @return int index of best matching server
 */
int	Request::findServerNameMatch(std::vector<Server> const & servers, \
std::vector<int>	& matches)
{
	int							longestLeading = -1;
	size_t						overlapLeading = 0;
	int							longestTrailing = -1;
	size_t						overlapTrailing = 0;
	std::vector<std::string>	hostSplit;
	
	// std::cout << "[FINDSERVERMATCH] about to split " << this->_hostname << std::endl;
	splitServerName(this->_hostname, hostSplit);
	for (auto it = matches.begin(); it != matches.end(); it++)
	{
		std::vector<std::string> const &	names = servers[*it].getServerNames();
		for (auto itName = names.begin(); itName != names.end(); itName++)
		{
			if (this->_hostname == *itName)
				return (*it);
			
			std::vector<std::string>	nameSplit;
			splitServerName(*itName, nameSplit);
			if ((*itName)[0] == '*' && (*itName)[1] == '.') // als server name only a "*" is --> segfault
			{
				size_t	overlap = countOverlapLeading(hostSplit, nameSplit);
				// std::cout << this->_hostname << " and " << *itName << " overlap: " << overlap << std::endl;
				if (overlap > overlapLeading)
				{
					overlapLeading = overlap;
					longestLeading = *it;
				}
			}
			else if ((*itName)[(*itName).length() - 1] == '*' && \
			(*itName)[(*itName).length() - 2] == '.')
			{
				size_t	overlap = countOverlapTrailing(hostSplit, nameSplit);
				// std::cout << this->_hostname << " and " << *itName << " overlap: " << overlap << std::endl;
				if (overlap > overlapTrailing)
				{
					overlapTrailing = overlap;
					longestTrailing = *it;
				}
			}
		}
	}
	if (overlapLeading > 0)
		return (longestLeading);
	else if (overlapTrailing > 0)
		return (longestTrailing);
	else
		return (*matches.begin());
}


/**
 * @brief verifies whether the host indication in the request and the host 
 * (IP address) at which a server is listening are both localhost. 
 * Returns true if yes and false if either one is not a localhost
 * 
 * @param address IP address on which a server is listening
 * @return true - if both are localhost
 * @return false - if at least one is not localhost
 */
bool	Request::isLocalhost(std::string const &address)
{
	std::vector<std::string>	localhost = {"localhost", "127.0.0.1"};
	size_t						count = 0;
	for (size_t idx = 0; idx < localhost.size(); idx++)
	{
		if (localhost[idx] == this->_address || localhost[idx] == address)
			count++;
	} 
	return (count == 2 ? true : false);
}

// FOR DEBUGGING ONLY
void	Request::printServer(Server const & server)
{
	std::cout << "--- SERVER ---" << std::endl;
	std::cout << "Listens:\t\t";
	for (size_t i = 0; i < server.getListens().size(); i++)
		std::cout << server.getHost(i) << ":" << server.getPort(i) << "; ";
	std::cout << std::endl;
	std::cout << "Server names:\t";
	for (size_t i = 0; i < server.getServerNames().size(); i++)
		std::cout << server.getServerName(i) << "; ";
	std::cout << std::endl;
	std::cout << "Locations:\t";
	for (size_t i = 0; i < server.getLocations().size(); i++)
		std::cout << server.getLocation(i).getMatch() << "; ";
	std::cout << std::endl << std::endl;
}

/**
 * @brief prints the content of the request instance; useful for debugging, 
 * can be deleted before submitting the project
 */
void	Request::printRequest()
{
	std::map<std::string, std::string>::iterator it;

	std::cout << "\n\t***\n\t" << this->_method << " ";
	std::cout << this->_target << " " << this->_protocolVersion << std::endl;
	std::cout << "\tQuery string: " << this->_queryString << std::endl;
	std::cout << "\tBoundary: " << this->_boundary << std::endl;
	for (it = this->_headers.begin(); it != this->_headers.end(); it++)
		std::cout << "\t" << it->first << ": " << it->second << std::endl;
	std::cout << "\tBody length: " << this->getBodyLength() << std::endl;
	std::cout << "\t***\n" << std::endl;
}