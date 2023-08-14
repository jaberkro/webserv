#include "Request.hpp"
# include "responseCodes.hpp"
#include "Webserver.hpp"
#include <algorithm>


Request::Request(int connfd, std::string address) : \
_method (""), \
_target (""), \
_protocolVersion (""), \
_bodyLength (0), \
_connFD (connfd), \
_statusCode (OK), \
_address (address), \
_contentLength (0) {
// _totalBytesRead (0) {
	std::cout << "***REQUEST CONSTRUCTOR CALLED, connfd is " << connfd << " ***" << std::endl;
	makeLowercase(this->_address); // not sure this is necessary
}

Request::~Request(void) {}

Request::Request(Request &r) : \
_method (r.getMethod()), \
_target (r.getTarget()), \
_queryString (r.getQueryString()), \
_protocolVersion (r.getProtocolVersion()), \
_headers (r.getHeaders()), \
_bodyLength (r.getBodyLength()), \
_connFD (r.getConnFD()), \
_statusCode (r.getStatusCode()), \
_address (r.getAddress()), \
_port (r.getPort()),\
_hostname (r.getHostname()), \
_contentLength (r.getContentLength())
// _totalBytesRead (r.getTotalBytesRead()) 
{
	for (auto it = r.getBody().begin(); it != r.getBody().end(); it++)
	{
		this->_body.push_back(std::pair<std::vector<uint8_t>, size_t>(it->first, it->second));
	}

}

Request &	Request::operator=(Request &r)
{
	this->_method = r.getMethod();
	this->_target = r.getTarget();
	this->_queryString = r.getQueryString();
	this->_headers = r.getHeaders();
	this->_protocolVersion = r.getProtocolVersion();
	this->_bodyLength = r.getBodyLength();
	for (auto it = r.getBody().begin(); it != r.getBody().end(); it++)
	{
		this->_body.push_back(std::pair<std::vector<uint8_t>, size_t>(it->first, it->second));
	}
	this->_connFD = r.getConnFD();
	this->_statusCode = r.getStatusCode();
	this->_address = r.getAddress();
	this->_port = r.getPort();
	this->_hostname = r.getHostname();
	this->_contentLength = r.getContentLength();
	// this->_totalBytesRead = r.getTotalBytesRead();
	return (*this);
}

/**
 * @brief reads a request from the socket, splits it into separate lines 
	and sends each line to the corresponding parsing function for further processing
 * 
 */
void	Request::processReq(void) 
{
	uint8_t		socketBuffer[MAXLINE];
	std::string	processingBuffer, line;
	ssize_t		bytesRead = 0;
	bool		firstLineComplete = false;
	bool		headersComplete = false;

	std::memset(socketBuffer, 0, MAXLINE);
	

	if (this->_method == "")
	{
		while ((bytesRead = recv(this->_connFD, &socketBuffer, MAXLINE, 0)) > 0 && !headersComplete)
		{
			std::cout << "[headers loop] just read " << bytesRead << " bytes." << std::endl;
			for (ssize_t i = 0; i < bytesRead; i++)
				processingBuffer += static_cast<char>(socketBuffer[i]);
			std::cout << "[PROCESSING BUFFER IS NOW] >" << processingBuffer << "<" << std::endl;
			std::memset(socketBuffer, 0, MAXLINE);
			while (!headersComplete && processingBuffer.find('\n') < std::string::npos) // if a whole (first) line is in the buffer
			{
				extractStr(processingBuffer, line, processingBuffer.find_first_of('\n'));
				if (!firstLineComplete)
				{
					// std::cout << "[parsing start line:] " << line << std::endl;
					firstLineComplete = this->parseStartLine(line);
				}
				else if (!headersComplete)
				{
					this->parseFieldLine(line);
					if (processingBuffer.find("\r\n") == 0)
						headersComplete = true;
				}
				// else
				// 	std::cout << "[processReq - FL and headers complete but I'm stuck in the loop]" << std::endl;
			}
			// std::cout << "[processReq] Out of the inner loop, headersComplete is " << headersComplete << ", bytesRead is " << bytesRead << ", totalBytesRead is " << this->_totalBytesRead << ", Content Length is ";
			std::cout << this->_contentLength << std::endl;
			if (headersComplete || (this->_bodyLength > 0 && this->_bodyLength == this->_contentLength))
				break;
		}
		if (bytesRead < 0)
			std::cerr << "[processReq] READING FROM SOCKET WENT WRONG" << std::endl;
		if (this->_contentLength > 0)
		{
			processingBuffer.erase(0, 2);
			this->_bodyLength = processingBuffer.length();
			std::vector<uint8_t>	bodyChunk(processingBuffer.begin(), processingBuffer.end());
			this->_body.push_back(std::pair<std::vector<uint8_t>, size_t>(bodyChunk, this->_bodyLength));
			// this->_totalBytesRead = this->_bodyLength;
			std::cout << "Just finished headers; body is now " << this->_body.size() << " items long with total of " << this->_bodyLength << " characters and chunk size of " << this->_body[0].first.size() << std::endl;
		}
		processingBuffer.clear();
	}
	// std::cout << "[after processing headers] buffer is >" << processingBuffer << "<" << std::endl;

	// READING THE BODY
	while (this->_bodyLength < this->_contentLength)
	{
		size_t	sizeToRead = std::min(this->_contentLength - this->_bodyLength, static_cast<size_t>(MAXLINE));
		while ((bytesRead = recv(this->_connFD, &socketBuffer, sizeToRead, 0)) > 0)
		{
			// this->addBytesRead(bytesRead);
			std::cout << "Read " << bytesRead << " bytes, total is now " << this->_bodyLength << std::endl;
			// _body.append(socketBuffer);
			// std::cout << "[reading body] Just read (SB) >" << socketBuffer << "<" << std::endl;
			std::vector<uint8_t>	bodyChunk;
			for (ssize_t i = 0; i < bytesRead; i++)
				bodyChunk.push_back(socketBuffer[i]);
			std::cout << "Created a new vector of size " << bodyChunk.size() << " and bytesRead are " << bytesRead << std::endl;
			this->_body.push_back(std::pair<std::vector<uint8_t>, size_t>(bodyChunk, bytesRead));
			this->_bodyLength += bytesRead;
			std::cout << "Just added a chunk; body is now " << this->_body.size() << " items long with total of " << this->_bodyLength << " characters." << std::endl;
			std::memset(socketBuffer, 0, MAXLINE);
			sizeToRead = std::min(this->_contentLength - this->_bodyLength, static_cast<size_t>(MAXLINE - 1));
			if (this->_bodyLength == this->_contentLength)
				break;
		}
		if (bytesRead < 0)
		{
			std::cerr << "[processReq] (read "  << this->_bodyLength << "/" << this->_contentLength << "), still to be read: " << sizeToRead << "NOT leaving loop" << std::endl;
			// break;
		}
		else if (bytesRead == 0)
		{
			std::cerr << "[processReq] READ 0; total read body length is " << this->_bodyLength << ", contentlength is " << this->_contentLength << std::endl;
			break;
		}
	}

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

	// trim spaces incl \r -> maybe not necessary

	key = extractKey(line);
	value = extractValue(line);
	if (key == "Host")
		setHost(value);
	else if (key == "Content-Length")
		setContentLength(value);
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
				this->_statusCode = INTERNAL_SERVER_ERROR;
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
			std::string const & serverAddress = servers[idx].getHost(i);
			// std::cout << "[FINDHOSTMATCH] COMPARING " << servers[idx].getPort(i) << " with " << this->_port << " and " << serverAddress << " with " << this->_address << std::endl;
			if (servers[idx].getPort(i) == this->_port && \
			(serverAddress == this->_address || this->isLocalhost(serverAddress)))
			{
				// std::cout << "[FINDHOSTMATCH] matches: adding " << servers[idx].getServerName(0) << std::endl;
				matches.push_back(idx);
			}
			else if (servers[idx].getHost(i) == "0.0.0.0" && *zero < 0)	// ook port vergelijken
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
				// std::cout << "[leading *] ";
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
				// std::cout << "[trailing *] ";
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

std::string const &	Request::getQueryString() const
{
	return (this->_queryString);

}

void	Request::setQueryString(std::string queryString)
{
	this->_queryString = queryString;
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

size_t	Request::getContentLength() const
{
	return (this->_contentLength);
}

void	Request::setContentLength(std::string contentLength)
{
	this->_contentLength = std::stoul(contentLength);
}

// size_t	Request::getTotalBytesRead() const
// {
// 	return (this->_totalBytesRead);
// }

// void	Request::addBytesRead(size_t bytesRead)
// {
// 	this->_totalBytesRead += bytesRead;
// }

std::string const &	Request::getHostname() const
{
	return (this->_hostname);
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
	this->_hostname = extractKey(host);
	// makeLowercase(this->_address);
	this->_port = stoi(extractValue(host)); // what if exception?
}

std::vector<std::pair<std::vector<uint8_t>, size_t>> & Request::getBody()
{
	return (this->_body);
}

size_t	Request::getBodyLength() const
{
	return (this->_bodyLength);
}


int	Request::getConnFD() const
{
	return (this->_connFD);
}

std::map<std::string, std::string> &	Request::getHeaders()
{
	return (this->_headers);
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
	std::cout << "\n\t***" << std::endl;
	std::cout << "\t" << this->_method << " " << this->_target << " " << this->_protocolVersion << std::endl;
	std::cout << "\tQuery string:" << this->_queryString << std::endl;
	for (std::map<std::string,std::string>::iterator it = this->_headers.begin(); \
	it != this->_headers.end(); it++)
		std::cout << "\t" << it->first << ": " << it->second << std::endl;
	std::cout << "Body length: " << this->getBodyLength() << std::endl;
	std::cout << "\t***\n" << std::endl;
}
