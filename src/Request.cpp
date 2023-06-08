#include "Request.hpp"
#include "../include/server.hpp"
#include <sys/socket.h>


Request::Request(int connfd) : _connFD (connfd) {}

Request::~Request(void) {}

/* 
	This function parses the start line of a request and saves the data in the 
	corresponding member variables
 */

std::string	Request::getMethod() const
{
	return (this->_method);
}

std::string	Request::getTarget() const
{
	return (this->_target);

}

std::string	Request::getProtocolVersion() const
{
	return (this->_protocolVersion);

}

int	Request::getConnFD() const
{
	return (this->_connFD);
}


bool	Request::parseStartLine(std::string &line)
{
	size_t	end;

	// trim spaces incl \r -> maybe not necessary
	end = line.find_first_not_of(UPPERCASE);
	this->_method = line.substr(0, end);
	line.erase(0, end + 1);
	end = line.find_first_of(" ");
	this->_target = line.substr(0, end);
	line.erase(0, end + 1);
	this->_protocolVersion = line.substr(0, std::string::npos);
	line.erase(0, std::string::npos);
	// std::cout << "method is " << this->_method << std::endl;
	// std::cout << "target is " << this->_target << std::endl;
	// std::cout << "protocol version is " << this->_protocolVersion << std::endl;
	return (true);
}

/* 
	This function parses each line of a request's header and saves the data in the 
	corresponding member variables
 */
void	Request::parseFieldLine(std::string &line) 
{
	std::string	key, value;
	size_t		colon;
	std::map<std::string,std::string>::iterator	it;

	// trim spaces incl \r -> maybe not necessary
	colon = line.find_first_of(":");
	key = line.substr(0, colon);
	value = line.substr(colon + 1, std::string::npos);
	this->removeTrailingSpaces(value);
	it = this->_headers.find(key);
	if (it == this->_headers.end())
		this->_headers.insert(std::pair<std::string,std::string>(key, value));
	else
		it->second += ", " + value;
	line.erase(0, std::string::npos);
}

/* 
	This function reads a request from the socket, splits it into separate lines 
	and sends each line to the parsing function for further processing
 */
void	Request::processReq(void) 
{
	char		socketBuffer[MAXLINE];
	std::string	processingBuffer, line;
	int			n = 0;
	size_t		nlPos = 0;
	// bodyRead = 0;
	bool		firstLineComplete = false;
	bool		headersComplete = false;

	std::memset(socketBuffer, 0, MAXLINE);
	while ((n = recv(this->_connFD, &socketBuffer, MAXLINE - 1, 0)) > 0) 
	{
		processingBuffer += socketBuffer;
		while (!firstLineComplete)
		{
			this->extractStr(processingBuffer, line, processingBuffer.find_first_of('\n'));
			firstLineComplete = this->parseStartLine(line);
		}
		while (!headersComplete && (nlPos = processingBuffer.find_first_of('\n')) < std::string::npos) 
		{
			if (nlPos > processingBuffer.find(HEADER_END))
				headersComplete = true;
			this->extractStr(processingBuffer, line, nlPos);
			this->parseFieldLine(line);
		}
		// CODE TO BE ADDED FOR READING THE BODY

		// std::cout << "Parsed headers: " << std::endl;
		// for (std::map<std::string,std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
		// 	std::cout << it->first << " -> " << it->second << std::endl;
		if (processingBuffer == "\r\n")
			break;
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
}

/* 
	This function extracts a string up to the first \n character from a "buffer"
	into the "line" and removes that part of the string from the buffer
 */
void	Request::extractStr(std::string &buffer, std::string &line, size_t nlPos)
{
	line = buffer.substr(0, nlPos - 1);
	buffer.erase(0, nlPos + 1);
}

/* 
	This function removes the trailing spaces from the beginning and end of a string
 */
void	Request::removeTrailingSpaces(std::string &line)
{
	line.erase(0, line.find_first_not_of(SPACES));
	line.erase(line.find_last_not_of(SPACES) + 1, std::string::npos);
}

