#include "Response.hpp"
#include <iostream>

size_t	Response::getFileLength( void ) const
{
	return (this->_fileLength);
}

std::string	& Response::getFilePath(void)
{
	return (this->_filePath);
}

void	Response::setFilePath(std::string path)
{
	this->_filePath = path;
}

std::string	& Response::getPathInfo(void)
{
	return (this->_pathInfo);
}

void	Response::setPathInfo(std::string pathInfo)
{
	this->_pathInfo = pathInfo;
}

bool	Response::getIsReady(void)
{
	return (this->_isReady);
}

std::vector<Location>::const_iterator const & Response::getLocation(void) const
{
	return (this->_location);
}

Request &	Response::getRequest(void)
{
	return (this->_req);
}

CGI &	Response::getCgi(void)
{
	return (this->_cgi);
}

int	Response::getStatusCode(void)
{
	return (this->_statusCode);
}

void	Response::setStatusCode(int code)
{
	this->_statusCode = code;
}

std::string	Response::getFullResponse(void)
{
	return (this->_fullResponse);
}

std::string	Response::getMessage(void)
{
	return (this->_message);
}

void	Response::addToFullResponse(char *response, size_t length)
{
	std::string	chunk(response, length);
	std::cerr << "[add to full response:] response is ";
	std::cerr << response << "; len is " << length << std::endl;
	std::cerr << "chunk is " << chunk << std::endl;
	this->_fullResponse.append(chunk);
}

void	Response::addToFullResponse(std::string chunk)
{
	this->_fullResponse.append(chunk);
}

void	Response::setMessage(std::string message)
{
	this->_message = message;
}

size_t	const & Response::getState() const
{
	return (this->_state);
}

void	Response::setState(size_t state)
{
	this->_state = state;
}
