#include "Response.hpp"

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

bool	Response::getIsReady(void)
{
	return (this->_isReady);
}

std::vector<Location>::const_iterator	const & Response::getLocation(void) const
{
	return (this->_location);
}

Request &	Response::getRequest(void)
{
	return (this->_req);
}

int	Response::getStatusCode(void)
{
	return (this->_statusCode);
}

uint8_t *	Response::getFullResponse(void)
{
	return (this->_fullResponse);
}

void	Response::setFullResponse(uint8_t *response)
{
	this->_fullResponse = response;
}

