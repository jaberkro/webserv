#include "Response.hpp"

Response::Response(Request & req) : \
cgiOnKqueue(false), \
_req (req), \
_statusCode (req.getStatusCode()), \
_fileLength (0), \
_isReady (false), \
_location (), \
_state(PENDING), \
_cgi(req) {}

Response::~Response(void) {}

Response::Response(Response &r) : \
_req (r.getRequest()), \
_statusCode (r.getStatusCode()), \
_fileLength (r.getFileLength()), \
_isReady (r.getIsReady()), \
_location (r.getLocation()), \
_fullResponse (r.getFullResponse()), \
_message (r.getMessage()), 
_state (r.getState()), \
_cgi (r.getCgi()) {}

Response &	Response::operator=(Response & r)
{
	this->_req = r.getRequest();
	this->_statusCode = r.getStatusCode();
	this->_fileLength = r.getFileLength();
	this->_isReady = r.getIsReady();
	this->_fullResponse = r.getFullResponse();
	this->_location = r.getLocation();
	this->_message = r.getMessage();
	this->_state = r.getState();
	return (*this);
}
