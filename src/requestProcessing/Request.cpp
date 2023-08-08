#include "Request.hpp"
# include "responseCodes.hpp"
#include "Webserver.hpp"
#include <sys/socket.h>


Request::Request(int connfd, std::string address) : \
_method (""), \
_target (""), \
_protocolVersion (""), \
_body (""), \
_connFD (connfd), \
_statusCode (OK), \
_address (address), \
_contentLength (0), \
_totalBytesRead (0) {
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
_body (r.getBody()), \
_connFD (r.getConnFD()), \
_statusCode (r.getStatusCode()), \
_address (r.getAddress()), \
_port (r.getPort()),\
_hostname (r.getHostname()), \
_contentLength (r.getContentLength()), \
_totalBytesRead (r.getTotalBytesRead()) {}

Request &	Request::operator=(Request &r)
{
	this->_method = r.getMethod();
	this->_target = r.getTarget();
	this->_queryString = r.getQueryString();
	this->_headers = r.getHeaders();
	this->_protocolVersion = r.getProtocolVersion();
	this->_body = r.getBody();
	this->_connFD = r.getConnFD();
	this->_statusCode = r.getStatusCode();
	this->_address = r.getAddress();
	this->_port = r.getPort();
	this->_hostname = r.getHostname();
	this->_contentLength = r.getContentLength();
	this->_totalBytesRead = r.getTotalBytesRead();
	return (*this);
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
	// size_t		nlPos = 0;
	bool		firstLineComplete = false;
	bool		headersComplete = false;

	std::memset(socketBuffer, 0, MAXLINE);

/* 
	read a chunk
		while there is a \n --> extract line & process line
		no \n --> read again

		stop when:	pos of \n is > pos of header end
					all Content Length has been read
					EOF (= bytesRead = 0)
 */
	
	// DO: USE THE SOCKETBUFFER MORE THAN STD::STRING PROCESSING BUFFER
	
	while ((bytesRead = recv(this->_connFD, &socketBuffer, MAXLINE - 1, 0)) > 0 && !headersComplete)
	{
		std::cout << "[outer loop first & headers] just read " << bytesRead << " bytes." << std::endl;
		this->addBytesRead(bytesRead);
		processingBuffer += socketBuffer;
		// std::cout << "[PROCESSING BUFFER IS NOW] >" << processingBuffer << "<" << std::endl;
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
		std::cout << "[processReq] Out of the inner loop, headersComplete is " << headersComplete << ", bytesRead is " << bytesRead << ", totalBytesRead is " << this->_totalBytesRead << ", Content Length is ";
		std::cout << this->_contentLength << std::endl;
		if (headersComplete || this->_totalBytesRead == this->_contentLength)
			break;
	}
	if (bytesRead < 0)
		std::cerr << "[processReq] READING FROM SOCKET WENT WRONG" << std::endl;
	else
		std::cout << "I'M HERE" << std::endl;
	// std::cout << "[after processing headers] buffer is >" << processingBuffer << "<" << std::endl;
	if (this->_contentLength > 0)
	{
		this->_body.append(processingBuffer.substr(2, std::string::npos));
		// this->_totalBytesRead = this->_body.length();
		// std::cout << "\tJust rewrote the totalBytesRead to " << this->_totalBytesRead << std::endl;
	}
	else
		this->_body.append(processingBuffer);
	processingBuffer.clear();

/* OLD CODE - READING FIRST LINE AND HEADERS
	while ((bytesRead = recv(this->_connFD, &socketBuffer, MAXLINE - 1, 0)) > 0) 
	{
		processingBuffer += socketBuffer;
		totalBytesRead += bytesRead;
		std::memset(socketBuffer, 0, MAXLINE);
		while (!firstLineComplete)
		{
			extractStr(processingBuffer, line, processingBuffer.find_first_of('\n'));
			firstLineComplete = this->parseStartLine(line);
		}
		if (this->_statusCode != OK)
			break;
		// std::cout << "na statuscode check" << std::endl;
		//BS Ergens tussen hier en het volgende comment blijftie hangen bij POST!
		while (!headersComplete && (nlPos = processingBuffer.find_first_of('\n')) < std::string::npos) 
		{
			if (nlPos > processingBuffer.find(HEADER_END))
				headersComplete = true;
			extractStr(processingBuffer, line, nlPos);
			this->parseFieldLine(line);
			// std::cout << "in headerscompletecomplete loop" << std::endl;

		}
	
		if (headersComplete)
		{
			// std::cout << "Before erasing: >" << processingBuffer << "<" << std::endl;
			processingBuffer.erase(0, 2);
			// std::cout << "After erasing: >" << processingBuffer << "<" << std::endl;
			this->_body.append(processingBuffer);
			break; //Silenced to be able to get the body!
		}
	}
	*/

	// READING THE BODY
	std::cout << "Processing buffer: [" << processingBuffer << "]" << std::endl;
	if (this->_contentLength > 0)
	{
		size_t	sizeToRead = std::min(this->_contentLength - this->_totalBytesRead, static_cast<size_t>(MAXLINE - 1));
		while ((bytesRead = recv(this->_connFD, &socketBuffer, sizeToRead, 0)) > 0)
		{
			this->addBytesRead(bytesRead);
			std::cout << "Read " << bytesRead << " bytes, total is now " << this->_totalBytesRead << std::endl;
			_body.append(socketBuffer);
			std::memset(socketBuffer, 0, MAXLINE);
			sizeToRead = std::min(this->_contentLength - this->_totalBytesRead, static_cast<size_t>(MAXLINE - 1));
			if (this->_totalBytesRead == this->_contentLength)
				break;
		}
		if (bytesRead < 0)
			std::cerr << "[processReq] READING BODY FROM SOCKET WENT WRONG" << std::endl;
	}
	
	// OLD CODE READING BODY
	// try
	// {
	// 	if (headersComplete && contentLength > 0) //means there is a body to read
	// 	{
	// 		// size_t	counter = 1;
	// 		while (totalBytesRead < static_cast<size_t>(contentLength))
	// 		{
	// 			sizeToRead = std::min(this->_contentLength - totalBytesRead, static_cast<size_t>(MAXLINE - 1));
	// 			// std::cout << "Round " << counter++ << ": total read: " << totalBytesRead << ", content length: " << contentLength << ", size to read: " << sizeToRead << std::endl;
	// 			bytesRead = recv(this->_connFD, &socketBuffer, sizeToRead, 0);
	// 			// std::cout << "socketBuffer: [" << socketBuffer << "], bytesread: " << bytesRead << std::endl;
	// 			// std::cout << "Just read " << bytesRead << " bytes" << std::endl;
	// 			if (bytesRead < 0)
	// 				perror("RECV ERROR: ");
	// 			if (bytesRead <= 0)
	// 				break;

	// 			_body.append(socketBuffer);
	// 			totalBytesRead += bytesRead;
	// 			std::memset(socketBuffer, 0, MAXLINE);
	// 			// std::cout << "End of loop. Total read is " << totalBytesRead << std::endl;
	// 		}
	// 	// delete[] socketBuf;
	// 	}
	// 	// std::cout << "Body now: ->" << this->_body << "<-" << std::endl;
	// }
	// catch (const std::length_error& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// }

	// // if Content-Length specified (while received <= Content-Length)
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

size_t	Request::getTotalBytesRead() const
{
	return (this->_totalBytesRead);
}

void	Request::addBytesRead(size_t bytesRead)
{
	this->_totalBytesRead += bytesRead;
}

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
	// std::cout << "Body: [" << this->getBody() << "]" << std::endl;
	std::cout << "\t***\n" << std::endl;
}
