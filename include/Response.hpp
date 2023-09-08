#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include "Request.hpp"
# include "responseCodes.hpp"
# include "Location.hpp"
// # include "Webserver.hpp"

#define DEFAULT_ERROR_PAGE "/defaultError.html"

class Response {

	public:

		Response(Request & req);
		~Response(void);
		Response(Response &);
		Response &	operator=(Response &);
		
		/* functions */
		void	prepareTargetURI(Server const & server);
		void	sendResponse(void);
		void	prepareResponsePOST(void);
		void	prepareResponseGET(void);
		void	prepareResponseDELETE(void);
		void	deleteFile(void);
		
		/* getters */
		size_t											getFileLength(void) const;
		Request &										getRequest(void);
		bool											getIsReady(void);
		int												getStatusCode(void);
		void											setStatusCode(int code);
		std::string	&									getFilePath(void);
		void											setFilePath(std::string path);
		std::vector<Location>::const_iterator	const & getLocation(void) const;
		std::string										getFullResponse(void);
		std::string										getMessage(void);
		void											setMessage(std::string);
		void											addToFullResponse(char *response, size_t length);
		void											addToFullResponse(std::string chunk);

		/* utils */
		void	splitUri(std::string const & uri, std::vector<std::string> & chunks);
		void	printResponse(void) const;	// for debugging purposes

	private:

		Request									_req;
		int										_statusCode;
		size_t									_fileLength;
		std::string								_filePath;
		bool									_isReady;
		std::vector<Location>::const_iterator	_location;
		std::string								_fullResponse;
		std::string								_message;
		static std::map<int, std::string>		_responseCodes;
		
		size_t									getFileSize(std::string filePath);
		std::string								identifyErrorPage(std::map<int, std::string> const & errorPages);
		std::vector<Location>::const_iterator 	findLocationMatch(std::string target, std::vector<Location> const & locations);
		std::vector<Location>::const_iterator	findExactLocationMatch(std::string target, std::vector<Location> const & locations);
		std::vector<Location>::const_iterator	findClosestLocationMatch(std::string target, std::vector<Location> const & locations);
		std::string								findIndexPage(std::vector<Location>::const_iterator itLoc);
		void									checkWhetherFileExists(void);
		void									prepareFirstLine(void);
		void									prepareHeaders(std::string const & root);
		void									prepareContent(void);
};

// std::string	deleteFile(Request request, std::vector<Location>::const_iterator const & location);
// bool allowedToDelete(std::string toRemove, std::vector<Location>::const_iterator const & location);
bool forbiddenToDeleteFileOrFolder(std::string toRemove);


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