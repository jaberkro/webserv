#include "Request.hpp"

std::string const &	Request::getMethod() const
{
	return (this->_method);
}

std::string const &	Request::getTarget() const
{
	return (this->_target);
}

std::string const &	Request::getQueryString() const
{
	return (this->_queryString);
}

std::string const &	Request::getBoundary() const
{
	return (this->_boundary);
}

std::string const &	Request::getProtocolVersion() const
{
	return (this->_protocolVersion);
}

int	Request::getStatusCode() const
{
	return (this->_statusCode);
}

size_t	Request::getContentLength() const
{
	return (this->_contentLength);
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

std::string Request::getBody()
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

size_t	const & Request::getState() const
{
	return (this->_state);
}

