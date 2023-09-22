#include "Request.hpp"
#include "responseCodes.hpp"
#include "Webserver.hpp"

std::map<int, std::string> 	Request::_requestStates = 
{
	{READHEADERS, "Readheaders"},
	{READBODY, "Readbody"},
	{WRITE, "Write"},
	{OVERWRITE, "Overwrite"},
	{REQ_ERROR, "Request Error"},
};

/**
 * @brief reads a request from the socket, splits it into separate lines and 
 * sends each line to the corresponding parsing function for further processing
 * 
 */
void	Request::processReq(int dataSize) 
{
	if (this->_state == READHEADERS)
		readFirstLineAndHeaders(dataSize);
	if (this->_state == READBODY)
		readBody(dataSize);
}

void	Request::readFirstLineAndHeaders(int &dataSize) 
{
	char		socketBuffer[MAXLINE];
	std::string	processingBuffer;
	ssize_t		bytesRead = 0;

	std::memset(socketBuffer, 0, MAXLINE);
	if (this->_state == READHEADERS && (bytesRead = recv(this->_connFD, \
	&socketBuffer, MAXLINE, 0)) > 0)
	{
		std::string	chunk(socketBuffer, bytesRead);
		processingBuffer += chunk;
		std::memset(socketBuffer, 0, MAXLINE);
		this->parseLines(processingBuffer);
	}
	if (bytesRead < 0 && dataSize > 0)
		setStatusCode(INTERNAL_SERVER_ERROR);
	if (bytesRead == 0)
		this->setState(OVERWRITE);
	if (this->_state == READBODY && this->_contentLength > 0)
	{
		this->_body = processingBuffer.substr(2);
		this->_bodyLength = _body.length();
		dataSize = 0;	// DM: this is to prevent getting -1 during the first reading of the body
	}
	processingBuffer.clear();
}

void	Request::parseLines(std::string & processingBuffer)
{
	std::string	line;

	while (this->_state == READHEADERS && \
	processingBuffer.find('\n') < std::string::npos)
	{
		extractStr(processingBuffer, line, processingBuffer.find_first_of('\n'));
		if (this->_method.empty())
			this->parseStartLine(line);
		else
		{
			this->parseFieldLine(line);
			if (processingBuffer.find("\r\n") == 0)
				this->setState(this->_method == "POST" ? READBODY : WRITE);
		}
	}
}

void		Request::readBody(int dataSize) 
{
	char	socketBuffer[MAXLINE];
	ssize_t	bytesRead = 0;
	
	std::memset(socketBuffer, 0, MAXLINE);
	if ((bytesRead = recv(this->_connFD, &socketBuffer, MAXLINE, 0)) > 0 && \
	this->_state != WRITE) // JMA: should we change the order of this if-statement?
	{
		std::string	chunk(socketBuffer, bytesRead);
		this->_body.append(chunk);
		this->_bodyLength += bytesRead;
		std::memset(socketBuffer, 0, MAXLINE);
		if (this->_bodyLength == this->_contentLength || \
		this->_body.find((this->_boundary + "--")) < std::string::npos)
			this->setState(WRITE);
	}
	if (bytesRead < 0 && dataSize > 0)
		setStatusCode(INTERNAL_SERVER_ERROR);
}

bool	Request::parseStartLine(std::string &line)
{
	size_t	end, questionMark;

	end = line.find_first_not_of(UPPERCASE);
	setMethod(line.substr(0, end));
	line.erase(0, end + 1);
	end = line.find_first_of(" ");
	setTarget(line.substr(0, end));
	line.erase(0, end + 1);
	questionMark = this->_target.find_first_of("?");
	if (questionMark < this->_target.length() - 1)
	{
		this->setQueryString(this->_target.substr(questionMark + 1, \
			std::string::npos));
		this->_target.erase(questionMark, std::string::npos);
	}
	if (this->_target.find("/..") < std::string::npos)
		this->setStatusCode(BAD_REQUEST);
	this->setProtocolVersion(line);
	line.clear();
	return (true);
}

void	Request::parseFieldLine(std::string &line) 
{
	std::string									key, value;
	std::map<std::string,std::string>::iterator	it;
	
	key = extractKey(line);
	value = extractValue(line);
	if (key == "Host")
		this->setHost(value);
	else if (key == "Content-Length")
		this->setContentLength(value);
	else if (key == "Content-Type" && value.find("boundary=") < std::string::npos)
		this->setBoundary(value.substr(value.find("boundary=") + 9));
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

Server const &	Request::identifyServer(std::vector<Server> const & servers)
{
	std::vector<size_t>	matches;
	size_t				bestMatch = 0;
	int					zero = -1;
	
	findHostMatch(servers, matches, &zero);
	switch (matches.size())
	{
		case 0:
			if (zero < 0){
				this->setStatusCode(INTERNAL_SERVER_ERROR); // JMA: should we also set the state?
			}
			return (servers[zero]);
		case 1:
			return (servers[matches[0]]);
		default:
			bestMatch = findServerNameMatch(servers, matches);
			return (servers[bestMatch]);
	}
}

void	Request::findHostMatch(std::vector<Server> const & servers, \
std::vector<size_t> & matches, int *zero)
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

size_t	Request::findServerNameMatch(std::vector<Server> const & servers, \
std::vector<size_t>	& matches)
{
	std::vector<size_t>::iterator	it;

	it = findExactServerNameMatch(servers, matches);
	if (it == matches.end())
	{
		std::vector<std::string>	hostSplit = splitServerName(this->_hostname);
		it = findLeadingServerNameMatch(servers, matches, hostSplit);
		if (it == matches.end())
			it = findTrailingServerNameMatch(servers, matches, hostSplit);
		if (it == matches.end())
			return (*matches.begin());
	}
	return (*it);
}

std::vector<size_t>::iterator	Request::findExactServerNameMatch(std::vector<Server> \
const & servers, std::vector<size_t>	& matches) 
{
	std::vector<size_t>::iterator	it;

	for (it = matches.begin(); it != matches.end(); it++)
	{
		std::vector<std::string> const &	names = servers[*it].getServerNames();
		for (auto itName = names.begin(); itName != names.end(); itName++)
		{
			if (this->_hostname == *itName)
				return (it);
		}
	}
	return (it);
}

std::vector<size_t>::iterator	Request::findLeadingServerNameMatch(std::vector<Server> \
const & servers, std::vector<size_t> & matches, std::vector<std::string> & hostSplit) 
{
	size_t							maxOverlap = 0;
	std::vector<size_t>::iterator	it;
	std::vector<size_t>::iterator	longest = matches.end();

	for (it = matches.begin(); it != matches.end(); it++)
	{
		std::vector<std::string> const &	names = servers[*it].getServerNames();
		for (auto itName = names.begin(); itName != names.end(); itName++)
		{
			std::vector<std::string>	srvrNameSplit = splitServerName(*itName);
			if ((*itName)[0] == '*' && itName->length() > 1 && (*itName)[1] == '.')
			{
				size_t	overlap = countOverlapLeading(hostSplit, srvrNameSplit);
				if (overlap > maxOverlap)
				{
					maxOverlap = overlap;
					longest = it;
				}
			}
		}
	}
	return (longest);
}

std::vector<size_t>::iterator	Request::findTrailingServerNameMatch(std::vector<Server> \
const & servers, std::vector<size_t> & matches, std::vector<std::string> & hostSplit) 
{
	size_t							maxOverlap = 0;
	std::vector<size_t>::iterator	it;
	std::vector<size_t>::iterator	longest = matches.end();

	for (it = matches.begin(); it != matches.end(); it++)
	{
		std::vector<std::string> const &	names = servers[*it].getServerNames();
		for (auto itName = names.begin(); itName != names.end(); itName++)
		{
			std::vector<std::string>	srvrNameSplit = splitServerName(*itName);
			if ((*itName)[(*itName).length() - 1] == '*' && \
			(*itName)[(*itName).length() - 2] == '.')
			{
				size_t	overlap = countOverlapTrailing(hostSplit, srvrNameSplit);
				if (overlap > maxOverlap)
				{
					maxOverlap = overlap;
					longest = it;
				}
			}
		}
	}
	return (longest);
}

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
