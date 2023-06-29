#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>

/* HOW TO INCORPORATE THIS IN server.cpp:

	#include "Request.hpp"
	
	add the following two lines in the while(1) loop below the line calling the accept() function
		Request	req(connfd);
		req.processReq();

	+ add Request.cpp in the Makefile
 */

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

		void								processReq(void);
		void								parseFieldLine(std::string &line);
		bool								parseStartLine(std::string &line);
		std::string	const &					getMethod() const;
		std::string	const &					getTarget() const;
		std::string	const &					getProtocolVersion() const;
		std::string	const &					getBody() const;
		int									getConnFD() const;
		std::map<std::string, std::string>	&getHeaders();

		void								printRequest();

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

};

#endif