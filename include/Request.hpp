#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Server.hpp"
# include <string>
# include <map>
# include <sys/socket.h>

# define HEADER_END "\r\n\r\n"
# define SPACES " \t\v\r\f"
# define UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

# define MAXLINE 6000
# define RESPONSELINE 5000 //test, was 5000

enum req {
	READHEADERS,
	READBODY,
	WRITE,
	OVERWRITE,
	REQ_ERROR
};

class Request 
{
	public:
		Request(int connfd, std::string address);
		~Request(void);
		Request(Request &);
		Request &	operator=(Request &);

		void			processReq(int dataSize);
		Server const &	identifyServer(std::vector<Server> const & servers);
	
		std::string	const &						getMethod() const;
		std::string	const &						getTarget() const;
		std::string	const &						getQueryString() const;
		std::string	const &						getBoundary() const;
		std::string	const &						getProtocolVersion() const;
		std::string const &						getHostname() const;
		std::string const &						getAddress() const;
		unsigned short							getPort() const;
		int										getStatusCode() const;
		size_t									getContentLength() const;
		std::string								getBody();
		size_t									getBodyLength() const;
		int										getConnFD() const;
		std::map<std::string, std::string>	 & 	getHeaders();
		size_t	const & 						getState() const;
		
		void	setMethod(std::string method);
		void	setTarget(std::string target);
		void	setQueryString(std::string queryString);
		void	setBoundary(std::string boundary);
		void	setProtocolVersion(std::string protocol);
		void	setStatusCode(int code);
		void	setContentLength(std::string contentLength);
		void	setHost(std::string host);
		void	setState(size_t state);
		void 	setBody(std::string newBody);
		void	setError(int statusCode);							

		void	printRequest();	// for debugging purposes, to be deleted
		void	printServer(Server const & server); // for debugging purposes, to be deleted

	private:
		std::string							_method;
		std::string							_target;
		std::string							_queryString;
		std::string							_boundary;
		std::string							_protocolVersion;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		size_t								_bodyLength;
		int									_connFD;
		int									_statusCode;
		std::string							_address;
		unsigned short						_port;
		std::string							_hostname;
		size_t								_contentLength;
		size_t								_state;
		static std::map<int, std::string>	_requestStates;
	
		void	readFirstLineAndHeaders(int dataSize);
		void	readBody(int dataSize);
		void	parseLines(std::string & processingBuffer);
		bool	parseStartLine(std::string &line);
		void	parseFieldLine(std::string &line);
		void	findHostMatch(std::vector<Server> const & servers, \
			std::vector<size_t> & matches, int *zero);
		size_t	findServerNameMatch(std::vector<Server> const & servers, \
			std::vector<size_t>	& matches);
		bool	isLocalhost(std::string const & address);
		std::vector<size_t>::iterator	findExactServerNameMatch(std::vector<Server> \
			const & servers, std::vector<size_t>	& matches);
		std::vector<size_t>::iterator	findLeadingServerNameMatch(std::vector<Server> \
			const & servers, std::vector<size_t> & matches, \
			std::vector<std::string> & hostSplit);
		std::vector<size_t>::iterator	findTrailingServerNameMatch(std::vector<Server> \
			const & servers, std::vector<size_t> & matches, \
			std::vector<std::string> & hostSplit);

};

void		removeTrailingSpaces(std::string &line);
std::string	extractKey(std::string line);
std::string	extractValue(std::string line);
void		extractStr(std::string &buffer, std::string &line, size_t pos);
size_t		countOverlapLeading(std::vector<std::string> & hostSplit, \
	std::vector<std::string> & nameSplit);
size_t		countOverlapTrailing(std::vector<std::string> & hostSplit, \
	std::vector<std::string> & nameSplit);
void		makeLowercase(std::string & str);
std::vector<std::string>	splitServerName(std::string const & name);

#endif
