#include "Response.hpp"
#include "Request.hpp"
#include <iostream>

void	Request::setMethod(std::string method)
{
	this->_method = method;
}

void	Request::setTarget(std::string target)
{
	this->_target = target;
}

void	Request::setQueryString(std::string queryString)
{
	this->_queryString = queryString;
}

void	Request::setBoundary(std::string boundary)
{
	this->_boundary = boundary;
}

void	Request::setProtocolVersion(std::string protocol)
{
	this->_protocolVersion = protocol;
}

void	Request::setStatusCode(int code)
{
	this->_statusCode = code;
	std::cout << "Changed request statuscode to: " << code;
	std::cout << ": " << Response::responseCodes[code] << std::endl;
}

void	Request::setContentLength(std::string contentLength)
{
	this->_contentLength = std::stoul(contentLength);
}

void	Request::setHost(std::string host)
{
	std::string	tmpHost = extractKey(host);
	if (!isLocalhost(tmpHost))
		this->_hostname = tmpHost;
	else
		this->_address = tmpHost;
	makeLowercase(this->_address);
	this->_port = stoi(extractValue(host)); // what if exception?
}

void	Request::setState(size_t state)
{
	this->_state = state;
	std::cout << "Changed request state to: " << state << ": ";
	std::cout << this->_requestStates[state] << std::endl;
}

void Request::setBody(std::string newBody)
{
	this->_body = newBody;
}

void	Request::setError(int statusCode)
{
	this->_state = REQ_ERROR;
	this->_statusCode = statusCode;
}
