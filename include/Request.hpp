#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include "Server.hpp"

# define HEADER_END "\r\n\r\n"
# define SPACES " \t\v\r\f"
# define UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

class Request 
{
	public:

		Request(int connfd, std::string address);
		~Request(void);
		Request(Request &);
		Request &	operator=(Request &);

		void		processReq(void);
		bool		parseStartLine(std::string &line);
		void		parseFieldLine(std::string &line);


		Server const &	identifyServer(std::vector<Server> const & servers);
		void			findHostMatch(std::vector<Server> const & servers, std::vector<int> & matches, int *zero);
		int				findServerNameMatch(std::vector<Server> const & servers, std::vector<int>	& matches);



		std::string	const &					getMethod() const;
		void								setMethod(std::string method);
		std::string	const &					getTarget() const;
		void								setTarget(std::string target);
		std::string	const &					getQueryString() const;
		void								setQueryString(std::string target);
		std::string	const &					getProtocolVersion() const;
		void								setProtocolVersion(std::string protocol);
		int									getStatusCode() const;
		void								setStatusCode(int code);
		size_t								getContentLength() const;
		void								setContentLength(std::string contentLength);
		size_t								getTotalBytesRead() const;
		void								addBytesRead(size_t bytesRead);
		std::string const &					getHostname() const;
		std::string const &					getAddress() const;
		unsigned short						getPort() const;
		void								setHost(std::string host);
		std::string	const &					getBody() const;
		int									getConnFD() const;
		std::map<std::string, std::string>	&getHeaders();
		std::string	const &					getFullRequest() const;

		bool	isLocalhost(std::string const & address);
		void	printRequest();	// for debugging purposes, to be deleted
		void	printServer(Server const & server); // for debugging purposes, to be deleted


	private:

		std::string					_method;
		std::string					_target;
		std::string					_queryString;
		std::string					_protocolVersion;
		std::map<std::string, std::string>	_headers;
		// std::map<std::string, std::string>	_trailers;
		std::string					_body;
		int							_connFD;
		int							_statusCode;
		std::string					_address;
		unsigned short				_port;
		std::string					_hostname;
		size_t						_contentLength;
		size_t						_totalBytesRead;
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