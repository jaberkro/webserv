#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include "Request.hpp"
# include "responseCodes.hpp"
# include "Location.hpp"
# include "CGI.hpp"
// # include "Webserver.hpp"

#define DEFAULT_ERROR_PAGE "/defaultError.html"

enum {
	PENDING,
	SENDING,
	DONE,
	WRITE_CGI,
	READ_CGI,
	INIT_CGI,
};

typedef std::vector<Location>::const_iterator locIterator;

class CGI;

class Response {

	public:

		Response(Request & req);
		~Response(void);
		Response(Response &);
		Response &	operator=(Response &);

		bool	cgiOnKqueue;
		
		/* functions */
		void	prepareTargetURI(Server const & server);
		void	performPOST(void);
		void	performGET(void);
		void	performDELETE(void);
		void	prepareResponse(Server const & server);
		void	sendResponse(void);
		void	identifyErrorPage(Server const & server);
		
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
		std::string			getFullResponse(void);
		std::string			getMessage(void);
		void				setMessage(std::string);
		size_t	const & 	getState() const;
		void				setState(size_t state);
		void				addToFullResponse(char *response, size_t length);
		void				addToFullResponse(std::string chunk);

		/* utils */
		void	splitUri(std::string const & uri, std::vector<std::string> & chunks);
		void	printResponse(void) const;	// for debugging purposes

	private:

		Request								_req;
		int									_statusCode;
		size_t								_fileLength;
		std::string							_filePath;
		bool								_isReady;
		locIterator							_location;
		std::string							_fullResponse;
		std::string							_message;
		static std::map<int, std::string>	_responseCodes;
		size_t								_state;
		std::string							_pathInfo;
		CGI									_cgi;
	
		size_t		getFileSize(std::string filePath);
		locIterator	findLocationMatch(std::string target, std::vector<Location> const & locations);
		locIterator	findExactLocationMatch(std::string target, std::vector<Location> const & locations);
		locIterator	findWildcardLocationMatch(std::string target, std::vector<Location> const & locations);
		locIterator	findClosestLocationMatch(std::string target, std::vector<Location> const & locations);
		std::string	findIndexPage(locIterator itLoc);
		void		extractPathInfo(std::string & targetUri);
		void		prepareFirstLine(void);
		void		prepareHeaders(std::string const & root);
		void		prepareContent(std::ifstream &file);
		void		executeCgiScript(void);
};

// std::string	deleteFile(Request request, locIterator const & location);
// bool allowedToDelete(std::string toRemove, locIterator const & location);
int			deleteFile(Request req, locIterator loc);
std::string createRemovePath(Request req, locIterator loc);
bool		forbiddenToDeleteFileOrFolder(std::string toRemove);
std::string	createAutoindex(void);
bool		hasReadPermission(std::string filePath);

#endif