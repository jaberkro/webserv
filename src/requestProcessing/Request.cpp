#include "Request.hpp"
# include "responseCodes.hpp"
#include "Webserver.hpp"
#include <sys/socket.h>


Request::Request(int connfd) : \
_method (""), \
_target (""), \
_protocolVersion (""), \
_body (""), \
_connFD (connfd), \
_statusCode (OK) {
	std::cout << "***REQUEST CONSTRUCTOR CALLED***" << std::endl;
}

Request::~Request(void) {}

Request::Request(Request &r) : \
_method (r.getMethod()), \
_target (r.getTarget()), \
_protocolVersion (r.getProtocolVersion()), \
_headers (r.getHeaders()), \
_body (r.getBody()), \
_connFD (r.getConnFD())  {}

Request &	Request::operator=(Request &r)
{
	this->_connFD = r.getConnFD();
	this->_method = r.getMethod();
	this->_target = r.getTarget();
	this->_protocolVersion = r.getProtocolVersion();
	this->_headers = r.getHeaders();
	this->_body = r.getBody();
	return (*this);
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
	size_t	end;

	end = line.find_first_not_of(UPPERCASE);
	setMethod(line.substr(0, end));
	line.erase(0, end + 1);
	end = line.find_first_of(" ");
	setTarget(line.substr(0, end));	// WATCH OUT: TARGET CAN BE AN ABSOLUTE PATH
	line.erase(0, end + 1);
	if (this->_target.find("/..") < std::string::npos)
		setStatusCode(BAD_REQUEST);
	setProtocolVersion(line.substr(0, std::string::npos));
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

	// trim spaces incl \r -> maybe not necessary

	key = extractKey(line);
	value = extractValue(line);
	if (key == "Host")
		setHost(value);
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
 * @brief extracts and returns the string preceding the first ':' character in 
 * the string given as an argument
 * 
 * @param line the string from which the string preceding the first ':' is to be
 * extracted
 * @return std::string - the extracted string
 */
std::string	Request::extractKey(std::string line)
{
	size_t	colon = line.find_first_of(":");
	
	return (line.substr(0, colon));
}

/**
 * @brief extracts and returns the string immediately following the first ':' 
 * character in the string given as an argument. The function removes trailing
 * spaces.
 * 
 * @param line the string from which the string following the first ':' is to be
 * extracted
 * @return std::string - the extracted string
 */
std::string	Request::extractValue(std::string line)
{
	size_t		colon = line.find_first_of(":");
	std::string	value;
	
	value = colon == std::string::npos ? "80" : line.substr(colon + 1, std::string::npos);
	this->removeTrailingSpaces(value);
	return (value);
	// RETURNS 80 IF EMPTY -> COULD BE MOVED TO A SEPARATE FUNCTION
}

/**
 * @brief reads a request from the socket, splits it into separate lines 
	and sends each line to the corresponding parsing function for further processing
 * 
 */
void	Request::processReq(void) 
{
	char		socketBuffer[MAXLINE];
	std::string	processingBuffer, line;
	// int			n = 0;
	ssize_t		bytesRead = 0;
	size_t		totalBytesRead = 0;
	size_t		nlPos = 0;
	bool		firstLineComplete = false;
	bool		headersComplete = false;

	std::memset(socketBuffer, 0, MAXLINE);
	while ((bytesRead = recv(this->_connFD, &socketBuffer, MAXLINE - 1, 0)) > 0) 
	{
		processingBuffer += socketBuffer;
		fullRequest += socketBuffer;
		totalBytesRead += bytesRead;
		std::memset(socketBuffer, 0, MAXLINE);
		while (!firstLineComplete)
		{
			this->extractStr(processingBuffer, line, processingBuffer.find_first_of('\n'));
			firstLineComplete = this->parseStartLine(line);
		}
		if (this->_statusCode != OK)
			break;
		while (!headersComplete && (nlPos = processingBuffer.find_first_of('\n')) < std::string::npos) 
		{
			if (nlPos > processingBuffer.find(HEADER_END))
				headersComplete = true;
			this->extractStr(processingBuffer, line, nlPos);
			this->parseFieldLine(line);
		}
		// CODE TO BE ADDED FOR READING THE BODY
	
		if (headersComplete)
		{
			// std::cout << "Before erasing: >" << processingBuffer << "<" << std::endl;
			processingBuffer.erase(0, 2);
			// std::cout << "After erasing: >" << processingBuffer << "<" << std::endl;
			this->_body.append(processingBuffer);
			processingBuffer.clear();
			break; //Silenced to be able to get the body!
		}
	}
	// std::cout << "Processing buffer: [" << processingBuffer << "]" << std::endl;
	std::string contentLengthStr = _headers["Content-Length"];
	int contentLength = atoi(contentLengthStr.c_str());
	setenv("CONTENT-LENGTH", contentLengthStr.c_str(), 0);

	std::cout << "Contentlen: " << contentLength << std::endl;
	try
	{
		if (headersComplete && contentLength > 0) //means there is a body to read
		{
			size_t	sizeToRead = MAXLINE - 1;
			size_t	counter = 1;
			while (totalBytesRead < static_cast<size_t>(contentLength))
			{
				sizeToRead = std::min(contentLength - totalBytesRead, static_cast<size_t>(MAXLINE - 1));
				std::cout << "Round " << counter++ << ": total read: " << totalBytesRead << ", content length: " << contentLength << ", size to read: " << sizeToRead << std::endl;
				bytesRead = recv(this->_connFD, &socketBuffer, sizeToRead, 0);
				// std::cout << "socketBuffer: [" << socketBuffer << "], bytesread: " << bytesRead << std::endl;
				std::cout << "Just read " << bytesRead << " bytes" << std::endl;
				if (bytesRead < 0)
					perror("RECV ERROR: ");
				if (bytesRead <= 0)
					break;

				_body.append(socketBuffer);
				fullRequest.append(socketBuffer);
				totalBytesRead += bytesRead;
				std::memset(socketBuffer, 0, MAXLINE);
				std::cout << "End of loop. Total read is " << totalBytesRead << std::endl;
			}
		// delete[] socketBuf;
		}
		std::cout << "Body now: ->" << this->_body << "<-" << std::endl;
	}
	catch (const std::length_error& e)
	{
		std::cerr << e.what() << '\n';
	}


	// if Content-Length specified (while received <= Content-Length)
	// while (firstLineComplete & headersComplete)
	// {
	// 	bodyRead += processingBuffer.length();
	// 	extractStr(processingBuffer, this->_body, processingBuffer.length());
	// 	std::cout << "now in the body part" << std::endl;
	// 	if (bodyRead == 0) // replace 0 with content-length
	// 		return;
	// }
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
	
	// for (auto printIt = servers.begin(); printIt != servers.end(); printIt++)
	// {
	// 	printServer(*printIt);
	// }
	findHostMatch(servers, matches, &zero);
	/* begin debug code */
	// std::cout << "Found " << matches.size() << " matching servers" << std::endl; 
	// for (auto printIt = matches.begin(); printIt != matches.end(); printIt++)
	// {
	// 	// printServer(*(*printIt));
	// 	std::cout << servers[*printIt].getServerName(0) << "; ";
	// }
	// std::cout << std::endl;
	// if (zero >= 0)
	// 	std::cout << "Zero is " << servers[zero].getServerName(0) << std::endl;
	// else
	// 	std::cout << "Zero is NOT there" << std::endl;
	/* end debug code */
	switch (matches.size())
	{
		case 0:
			if (zero < 0)
				throw std::runtime_error("ERROR: No matching server, not even a default 0.0.0.0 found");
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
			std::string const & reqAddress = servers[idx].getHost(i);
			if (servers[idx].getPort(i) == this->_port && \
			(reqAddress == this->_address || isLocalhost(reqAddress)))
				matches.push_back(idx);
			if (servers[idx].getHost(i) == "0.0.0.0" && *zero < 0)
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
	
	splitServerName(this->_headers.at("Host"), hostSplit);
	for (auto it = matches.begin(); it != matches.end(); it++)
	{
		std::vector<std::string> const &	names = servers[*it].getServerNames();
		for (auto itName = names.begin(); itName != names.end(); itName++)
		{
			if (this->_headers.at("Host") == *itName)
				return (*it);
			
			std::vector<std::string>	nameSplit;
			splitServerName(*itName, nameSplit);
			if ((*itName)[0] == '*' && (*itName)[1] == '.')
			{
				// std::cout << "[leading *] ";
				size_t	overlap = countOverlapLeading(hostSplit, nameSplit);
				// std::cout << this->_headers.at("Host") << " and " << *itName << " overlap: " << overlap << std::endl;
				if (overlap > overlapLeading)
				{
					overlapLeading = overlap;
					longestLeading = *it;
				}
			}
			else if ((*itName)[(*itName).length() - 1] == '*' && \
			(*itName)[(*itName).length() - 2] == '.')
			{
				// std::cout << "[trailing *] ";
				size_t	overlap = countOverlapTrailing(hostSplit, nameSplit);
				// std::cout << this->_headers.at("Host") << " and " << *itName << " overlap: " << overlap << std::endl;
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
 * @brief counts the overlapping elements in hostnames starting with a leading
 * asterisk
 * 
 * @param hostSplit hostname in the request, split by element ('.' or string)
 * @param nameSplit server_name, split by element ('.' or string)
 * @return size_t index of the server with longest overlap
 */
size_t	Request::countOverlapLeading(std::vector<std::string> & hostSplit, \
std::vector<std::string> & nameSplit)
{
	size_t	revIdxN = nameSplit.size() - 1;
	size_t	revIdxH = hostSplit.size() - 1;

	for (size_t idx = 0; idx < hostSplit.size(); idx++)
	{
		if (nameSplit[revIdxN - idx] == "*")
			return (idx);
		if (nameSplit[revIdxN - idx] != hostSplit[revIdxH - idx])
			break;
	}
	return (0);
}

/**
 * @brief counts the overlapping elements in hostnames starting with a trailing
 * asterisk
 * 
 * @param hostSplit hostname in the request, split by element ('.' or string)
 * @param nameSplit server_name, split by element ('.' or string)
 * @return size_t index of the server with longest overlap
 */
size_t	Request::countOverlapTrailing(std::vector<std::string> & hostSplit, \
std::vector<std::string> & nameSplit)
{
	for (size_t idx = 0; idx < hostSplit.size(); idx++)
	{
		if (nameSplit[idx] == "*")
			return (idx);
		if (nameSplit[idx] != hostSplit[idx])
			break;
	}
	return (0);		
}

/**
 * @brief splits a string (server name) into elements of '.' and strings of other
 * characters
 * 
 * @param name domain name to be split
 * @param chunks vector in which the elements are saved
 */
void	Request::splitServerName(std::string const & name, std::vector<std::string> & chunks)
{
	size_t	begin = 0;
	size_t	end;

	while (begin < name.length())
	{
		end = name[begin] == '.' ? name.find_first_of('.', begin) + 1 : name.find_first_of('.', begin);
		chunks.push_back(name.substr(begin, end - begin));
		begin = end;
	}
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
 * @brief extracts a substring from beginning of a buffer until the following
 * new line (at the index of nlPos), deletes it from the buffer and moves it 
 * into the line. The new line character is discarded in the process.
 * 
 * @param buffer source of the to be extracted string; the string will be removed
 * from the buffer
 * @param line destination in which the extracted string is placed
 * @param nlPos denotes the position of the closest new line character
 */
void	Request::extractStr(std::string &buffer, std::string &line, size_t nlPos)
{
	line = buffer.substr(0, nlPos - 1);
	buffer.erase(0, nlPos + 1);
}

/**
 * @brief removes the trailing spaces from the beginning and end of a string
 * 
 * @param line string, from which trailing spaces should be removed
 */
void	Request::removeTrailingSpaces(std::string &line)
{
	line.erase(0, line.find_first_not_of(SPACES));
	line.erase(line.find_last_not_of(SPACES) + 1, std::string::npos);
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

/**
 * @brief transforms a string into lowercase
 * 
 * @param str reference to a string to be transformed into lowercase
 */
void	Request::makeLowercase(std::string & str)
{
	for (size_t idx = 0; idx < str.length(); idx++)
		str[idx] = tolower(str[idx]);
}

/**
 * @brief prints the content of the request instance; useful for debugging, 
 * can be deleted before submitting the project
 */
void	Request::printRequest()
{
	std::cout << "\n\t***" << std::endl;
	std::cout << "\t" << this->_method << " " << this->_target << std::endl;
	for (std::map<std::string,std::string>::iterator it = this->_headers.begin(); \
	it != this->_headers.end(); it++)
		std::cout << "\t" << it->first << ": " << it->second << std::endl;
	std::cout << "Body: [" << this->getBody() << "]" << std::endl;
	std::cout << "\t***\n" << std::endl;
}


/* GETTERS & SETTERS */

std::string const &	Request::getMethod() const
{
	return (this->_method);
}

void	Request::setMethod(std::string method)
{
	this->_method = method;
}

std::string const &	Request::getTarget() const
{
	return (this->_target);

}

void	Request::setTarget(std::string target)
{
	this->_target = target;
}

std::string const &	Request::getProtocolVersion() const
{
	return (this->_protocolVersion);

}

void	Request::setProtocolVersion(std::string protocol)
{
	this->_protocolVersion = protocol;
}


int	Request::getStatusCode() const
{
	return (this->_statusCode);
}

void	Request::setStatusCode(int code)
{
	this->_statusCode = code;
}

std::string const &	Request::getAddress() const
{
	return (this->_address);
}

unsigned short	Request::getPort() const
{
	return (this->_port);
}

void	Request::setHost(std::string host)
{
	this->_address = this->extractKey(host);
	makeLowercase(this->_address);
	this->_port = stoi(this->extractValue(host)); // what if exception?
}

std::string	const & Request::getBody() const
{
	return (this->_body);
}

int	Request::getConnFD() const
{
	return (this->_connFD);
}

std::map<std::string, std::string> &	Request::getHeaders()
{
	return (this->_headers);
}

std::string	const & Request::getFullRequest() const
{
	return(this->fullRequest);
}