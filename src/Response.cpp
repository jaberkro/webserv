#include "Response.hpp"

Response::Response(Request &req) : req (req) {}

Response::~Response(void) {}

std::string	Response::createResponse(void)
{
	// if req->getHeader() Sec-Fetch-Dest is document ...
	// else image ...
}