#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <sys/socket.h>
# include "Server.hpp"

# define HEADER_END "\r\n\r\n"
# define SPACES " \t\v\r\f"
# define UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

# define MAXLINE 6000
# define RESPONSELINE 500 //test, was 5000

enum {
	READHEADERS,
	READBODY,
	WRITE,
	OVERWRITE
};

class Request 
{
	public:

		Request(int connfd, std::string address);
		~Request(void);
		Request(Request &);
		Request &	operator=(Request &);

		void			processReq(void);
		Server const &	identifyServer(std::vector<Server> const & servers);
	
		std::string	const &									getMethod() const;
		std::string	const &									getTarget() const;
		std::string	const &									getQueryString() const;
		std::string	const &									getBoundary() const;
		std::string	const &									getProtocolVersion() const;
		std::string const &									getHostname() const;
		std::string const &									getAddress() const;
		unsigned short										getPort() const;
		int													getStatusCode() const;
		size_t												getContentLength() const;
		std::string											getBody();
		size_t												getBodyLength() const;
		int													getConnFD() const;
		std::map<std::string, std::string>	 & 				getHeaders();
		size_t	const & 									getState() const;
		
		void												setMethod(std::string method);
		void												setTarget(std::string target);
		void												setQueryString(std::string queryString);
		void												setBoundary(std::string boundary);
		void												setProtocolVersion(std::string protocol);
		void												setStatusCode(int code);
		void												setContentLength(std::string contentLength);
		void												setHost(std::string host);
		void												setState(size_t state);
		void 												setBody(std::string newBody);

		void			printRequest();	// for debugging purposes, to be deleted
		void			printServer(Server const & server); // for debugging purposes, to be deleted


	private:

		std::string												_method;
		std::string												_target;
		std::string												_queryString;
		std::string												_boundary;
		std::string												_protocolVersion;
		std::map<std::string, std::string>						_headers;
		std::string												_body;
		size_t													_bodyLength;
		int														_connFD;
		int														_statusCode;
		std::string												_address;
		unsigned short											_port;
		std::string												_hostname;
		size_t													_contentLength;
		size_t													_state;
		
		void			readFirstLineAndHeaders(void);
		void			readBody(void);
		bool			parseStartLine(std::string &line);
		void			parseFieldLine(std::string &line);
		void			findHostMatch(std::vector<Server> const & servers, std::vector<int> & matches, int *zero);
		int				findServerNameMatch(std::vector<Server> const & servers, std::vector<int>	& matches);
		bool			isLocalhost(std::string const & address);
};

void			removeTrailingSpaces(std::string &line);
std::string		extractKey(std::string line);
std::string		extractValue(std::string line);
void			extractStr(std::string &buffer, std::string &line, size_t pos);
size_t			countOverlapLeading(std::vector<std::string> & hostSplit, std::vector<std::string> & nameSplit);
size_t			countOverlapTrailing(std::vector<std::string> & hostSplit, std::vector<std::string> & nameSplit);
void			makeLowercase(std::string & str);
void			splitServerName(std::string const & name, std::vector<std::string> & chunks);



#endif