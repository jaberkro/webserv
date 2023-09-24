#include "Request.hpp"
#include "responseCodes.hpp"

Request::Request(int connfd, std::string address) : \
_method (""), \
_target (""), \
_queryString (""), \
_protocolVersion (""), \
_bodyLength (0), \
_connFD (connfd), \
_statusCode (OK), \
_address (address), \
_contentLength (0), \
_state (READHEADERS) {
	makeLowercase(this->_address);
}

Request::Request(Request &r) : \
_method (r.getMethod()), \
_target (r.getTarget()), \
_queryString (r.getQueryString()), \
_boundary (r.getBoundary()), \
_protocolVersion (r.getProtocolVersion()), \
_headers (r.getHeaders()), \
_body (r.getBody()), \
_bodyLength (r.getBodyLength()), \
_connFD (r.getConnFD()), \
_statusCode (r.getStatusCode()), \
_address (r.getAddress()), \
_port (r.getPort()),\
_hostname (r.getHostname()), \
_contentLength (r.getContentLength()), \
_state(r.getState())
{}

Request &	Request::operator=(Request &r)
{
	this->_method = r.getMethod();
	this->_target = r.getTarget();
	this->_queryString = r.getQueryString();
	this->_boundary = r.getBoundary();
	this->_headers = r.getHeaders();
	this->_protocolVersion = r.getProtocolVersion();
	this->_bodyLength = r.getBodyLength();
	this->_body = r.getBody();
	this->_connFD = r.getConnFD();
	this->_statusCode = r.getStatusCode();
	this->_address = r.getAddress();
	this->_port = r.getPort();
	this->_hostname = r.getHostname();
	this->_contentLength = r.getContentLength();
	this->_state = r.getState();
	return (*this);
}

Request::~Request(void) {}
