#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <map>
# include "Request.hpp"

class Response {

	public:

		Response(Request req);
		~Response(void);
		void	createResponse(uint8_t *response);

	private:

		Request								_req;
		std::string							_statusCode;
		std::string							_reason;
		std::map<std::string, std::string>	_headers;
		std::string							_content;
		// std::string							_protocolVersion;
		// std::map<std::string, std::string>	_trailers;
		// int									_connFD;

};

/* 
RE CONTENT
For a response message, the following rules are applied in order until a match is found:
If the request method is HEAD or the response status code is 204 (No Content) or 304 (Not Modified), there is no content in the response.
If the request method is GET and the response status code is 200 (OK), the content is a representation of the target resource (Section 7.1).
If the request method is GET and the response status code is 203 (Non-Authoritative Information), the content is a potentially modified or enhanced representation of the target resource as provided by an intermediary.
If the request method is GET and the response status code is 206 (Partial Content), the content is one or more parts of a representation of the target resource.
If the response has a Content-Location header field and its field value is a reference to the same URI as the target URI, the content is a representation of the target resource.
If the response has a Content-Location header field and its field value is a reference to a URI different from the target URI, then the sender asserts that the content is a representation of the resource identified by the Content-Location field value. However, such an assertion cannot be trusted unless it can be verified by other means (not defined by this specification).
Otherwise, the content is unidentified by HTTP, but a more specific identifier might be supplied within the content itself.
 */

#endif