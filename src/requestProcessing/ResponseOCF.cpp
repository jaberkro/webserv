#include "Response.hpp"

/**
 * @brief Construct a new Response:: Response object
 * 
 * @param req reference to a Request instance
 */
Response::Response(Request & req) : \
_req (req), \
_statusCode (req.getStatusCode()), \
_fileLength (0), \
_isReady (false), \
_location () {}

/**
 * @brief Destroy the Response:: Response object
 * 
 */
Response::~Response(void) {}

/**
 * @brief Construct a new Response:: Response object
 * 
 * @param r reference to a Response instance to be copied
 */
Response::Response(Response &r) : \
_req (r.getRequest()), \
_statusCode (r.getStatusCode()), \
_fileLength (r.getFileLength()), \
_isReady (r.getIsReady()), \
_location (), \
_fullResponse (r.getFullResponse()),
_message (r.getMessage()) {}

/**
 * @brief Response copy assignment operator
 * 
 * @param r reference to a Response instance to be copied
 * @return Response& 
 */
Response &	Response::operator=(Response & r)
{
	this->_req = r.getRequest();
	this->_statusCode = r.getStatusCode();
	this->_fileLength = r.getFileLength();
	this->_isReady = r.getIsReady();
	this->_fullResponse = r.getFullResponse();
	this->_message = r.getMessage();

	return (*this);
}
