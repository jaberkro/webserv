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

		Request(int connfd);
		~Request(void);
		Request(Request &);
		Request &	operator=(Request &);

		void		processReq(void);
		bool		parseStartLine(std::string &line);
		void		parseFieldLine(std::string &line);
		std::string	extractKey(std::string line);
		std::string	extractValue(std::string line);


		Server const &	identifyServer(std::vector<Server> const & servers);
		void			findHostMatch(std::vector<Server> const & servers, std::vector<int> & matches, int *zero);
		int				findServerNameMatch(std::vector<Server> const & servers, std::vector<int>	& matches);
		void			splitServerName(std::string const & name, std::vector<std::string> & chunks);
		size_t			countOverlapLeading(std::vector<std::string> & hostSplit, std::vector<std::string> & nameSplit);
		size_t			countOverlapTrailing(std::vector<std::string> & hostSplit, std::vector<std::string> & nameSplit);



		std::string	const &					getMethod() const;
		void								setMethod(std::string method);
		std::string	const &					getTarget() const;
		void								setTarget(std::string target);
		std::string	const &					getProtocolVersion() const;
		void								setProtocolVersion(std::string protocol);
		int									getStatusCode() const;
		void								setStatusCode(int code);
		std::string const &					getAddress() const;
		unsigned short						getPort() const;
		void								setHost(std::string host);
		std::string	const &					getBody() const;
		int									getConnFD() const;
		std::map<std::string, std::string>	&getHeaders();
		bool	isLocalhost(std::string const & address);
		void	makeLowercase(std::string & str);

		void								printRequest();	// for debugging purposes, to be deleted
		void	printServer(Server const & server); // for debugging purposes, to be deleted


	private:

		void	extractStr(std::string &buffer, std::string &line, size_t pos);
		void	removeTrailingSpaces(std::string &line);
		std::string					_method;
		std::string					_target;
		std::string					_protocolVersion;
		std::map<std::string, std::string>	_headers;
		// std::map<std::string, std::string>	_trailers;
		std::string					_body;
		int							_connFD;
		int							_statusCode;
		std::string					_address;
		unsigned short				_port;
};


#endif