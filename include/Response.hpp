#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
# include "responseCodes.hpp"
# include "Location.hpp"
# include "CGI.hpp"

#define DEFAULT_ERROR_PAGE "/defaultError.html"

enum resp {
	PENDING,
	SENDING,
	DONE,
	WRITE_CGI,
	READ_CGI,
	INIT_CGI,
	RES_ERROR
};

typedef std::vector<Location>::const_iterator locIterator;

class Response {

	public:
		Response(Request & req);
		~Response(void);
		Response(Response &);
		Response &	operator=(Response &);

		bool								cgiOnKqueue;
		static std::map<int, std::string>	responseCodes;
		
		/* functions */
		void	processTarget(Server const & server);
		void	performRequest(void);
		void	performPOST(void);
		void	performGET(void);
		void	performDELETE(void);
		void	prepareResponse(Server const & server);
		void	sendResponse(void);
		void	identifyErrorPage(Server const & server);
		void	executeCgiScript(void);
		
		/* getters */
		size_t				getFileLength(void) const;
		Request &			getRequest(void);
		CGI &				getCgi(void);
		bool				getIsReady(void);
		int					getStatusCode(void);
		void				setStatusCode(int code);
		std::string	&		getFilePath(void);
		void				setFilePath(std::string path);
		std::string	&		getPathInfo(void);
		void				setPathInfo(std::string pathInfo);
		locIterator	const & getLocation(void) const;
		std::string			getScriptName(void);
		std::string			getFullResponse(void);
		std::string			getMessage(void);
		void				setMessage(std::string);
		std::string			getResponseCodeMessage(int responseCode);
		size_t	const & 	getState() const;
		void				setState(size_t state);
		void				addToFullResponse(char *response, size_t length);
		void				addToFullResponse(std::string chunk);
		void				setError(int statusCode);

	private:
		Request								_req;
		int									_statusCode;
		size_t								_fileLength;
		std::string							_filePath;
		bool								_isReady;
		locIterator							_location;
		std::string							_fullResponse;
		std::string							_message;
		size_t								_state;
		std::string							_pathInfo;
		CGI									_cgi;
		static std::map<int, std::string>	_responseStates;
	
		locIterator	findLocationMatch(std::string target, \
		std::vector<Location> const & locations);
		std::string	findIndexPage(locIterator itLoc);
		void		prepareFilePath(std::string & targetUri);
		void		prepareErrorPageFilePath(std::string & targetUri);
		void		extractPathInfo(std::string & targetUri);
		std::string	getErrorPageUri(void);
		void		prepareFirstLine(void);
		void		prepareHeaders(std::string const & root);
		std::string	prepareHeaderContentType(std::string const & root);
		size_t		prepareHeaderContentLength(void);
		void		prepareHeaderLocation(void);
		void		prepareContent(std::ifstream &file);
		void		checkIfRedirectNecessary(void);
		void		checkIfMethodAllowed(void);
		void		checkIfGetIsActuallyDelete(void);
		void		checkIfReturn(void);
		void		composeResponse(std::ifstream & file);
		void		checkIfFilePathIsDirectory();

};

int			deleteFile(Request req, locIterator loc);
std::string createRemovePath(Request req, locIterator loc);
bool		forbiddenToDeleteFileOrFolder(std::string toRemove);
std::string	createAutoindex(Response &response);
bool		hasReadPermission(std::string filePath);
locIterator	findExactLocationMatch(std::string target, \
std::vector<Location> const & locations);
locIterator	findWildcardLocationMatch(std::string target, \
std::vector<Location> const & locations);
locIterator	findClosestLocationMatch(std::string target, \
std::vector<Location> const & locations);
void		splitUri(std::string const & uri, std::vector<std::string> & chunks);
size_t		getFileSize(std::string filePath);
bool		isContentAllowed(int statusCode);
bool		isRequestedByCurl(std::string userAgent);

#endif
