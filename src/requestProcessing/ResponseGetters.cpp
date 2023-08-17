#include "Response.hpp"

size_t	Response::getFileLength( void ) const
{
	return (this->_fileLength);
}

std::string	& Response::getFilePath(void)
{
	return (this->_filePath);
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

