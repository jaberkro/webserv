#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <map>

/* HOW TO INCORPORATE THIS IN server.cpp:

	#include "Request.hpp"
	
	add the following two lines in the while(1) loop below the line calling the accept() function
		Request	req(connfd);
		req.processReq();

	+ add Request.cpp in the Makefile
 */

class Request {

	public:

		Request(int connfd);
		~Request(void);
		void	processReq(void);
		void	parse(std::string line);

	private:

		int									_connfd;	// fd from which the request is read
		const std::string					_method;
		const std::string					_target;
		const std::string					_protocolVersion;
		std::map<std::string, std::string>	_headers;
		// std::map<std::string, std::string>	_trailers;
		const std::string					_body;

};

#endif