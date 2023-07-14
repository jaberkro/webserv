#include "Request.hpp"
#include "Webserver.hpp"
#include <sys/socket.h>


Request::Request(int connfd) : \
_method (""), \
_target (""), \
_protocolVersion (""), \
_body (""), \
_connFD (connfd), \
_statusCode (200) {
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

std::string const &	Request::getMethod() const
{
	return (this->_method);
}

std::string const &	Request::getTarget() const
{
	return (this->_target);

}

std::string const &	Request::getProtocolVersion() const
{
	return (this->_protocolVersion);

}

std::string	const & Request::getBody() const
{
	return (this->_body);

}

int	Request::getStatusCode() const
{
	return (this->_statusCode);
}

int	Request::getConnFD() const
{
	return (this->_connFD);
}

std::map<std::string, std::string> &	Request::getHeaders()
{
	return (this->_headers);
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

	// trim spaces incl \r -> maybe not necessary
	end = line.find_first_not_of(UPPERCASE);
	this->_method = line.substr(0, end);
	line.erase(0, end + 1);
	end = line.find_first_of(" ");
	this->_target = line.substr(0, end);	// WATCH OUT: TARGET CAN BE AN ABSOLUTE PATH
	line.erase(0, end + 1);
	if (this->_target.find("/..") < std::string::npos)
		this->_statusCode = 400;
	this->_protocolVersion = line.substr(0, std::string::npos);
	line.erase(0, std::string::npos);
	// std::cout << "method is " << this->_method << std::endl;
	// std::cout << "target is " << this->_target << std::endl;
	// std::cout << "protocol version is " << this->_protocolVersion << std::endl;
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

/**
 * @brief reads a request from the socket, splits it into separate lines 
	and sends each line to the parsing function for further processing
 * 
 */
void	Request::processReq(void) 
{
	char		socketBuffer[MAXLINE];
	std::string	processingBuffer, line;
	int			n = 0;
	size_t		nlPos = 0;
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
		if (this->_statusCode != 200)
			break;
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
 * @brief prints the content of the request instance; useful for debugging, 
 * can be deleted before submitting the project
 */
void	Request::printRequest()
{
	std::cout << "***" << std::endl;
	std::cout << this->_method << " " << this->_target << std::endl;
	for (std::map<std::string,std::string>::iterator it = this->_headers.begin(); \
	it != this->_headers.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << "***" << std::endl;
}
