#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
# include "Request.hpp"
# include "Location.hpp"

class Response {

	public:

		Response(Request req);
		~Response(void);
		Response(Response &);
		Response &	operator=(Response &);
		
		uint8_t	*createResponse();
		void	retrieveImg(uint8_t *response);
		void	retrieveFile(uint8_t *response);
		void	prepareGetResponse(uint8_t *response, std::vector<Location> & locations);

		
		size_t	getMsgLength(void) const;
		void	splitUri(std::string const & uri, std::vector<std::string> & chunks);
		
		Request &		getRequest(void);
		bool			getIsReady(void);
		int				getStatusCode(void);
		std::string	& getFilePath(void);
		std::vector<Location>::iterator findMatch(std::string target, std::vector<Location> & locations);
		std::vector<Location>::iterator	findExactMatch(std::string target, std::vector<Location> & locations);
		std::vector<Location>::iterator	findClosestMatch(std::string target, std::vector<Location> & locations);

	private:

		Request								_req;
		int									_statusCode;
		std::string							_reason;
		std::map<std::string, std::string>	_headers;
		std::string							_content;
		size_t								_msgLength;
		std::string							_filePath;
		bool								_isReady;
		// std::map<std::string, std::string>	_trailers;
		size_t	getFileSize(std::string filePath);

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