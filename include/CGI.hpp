#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include "Request.hpp"

#define R 0
#define W 1

class Response;

class CGI
{
	public:

		CGI(Request & req);
		~CGI();
		CGI &	operator=(CGI & r);

		void	run(Response & response);
		void	prepareArg(std::string const & scriptName);
		void	prepareEnv(std::string const & scriptName, Response & response);
		int*	getWebservToScript();
		int*	getScriptToWebserv();
		bool	checkIfCgiPipe();
		void	cgiRead(Response & response, std::string & fullResponse, int dataSize);
		void	cgiWrite(Response & response, int dataSize);

		std::string	getResponse();

	private:

		Request									& _req;
		char									**_arg;
		char									**_env;
		pid_t									id;
		int										_webservToScript[2];
		int										_scriptToWebserv[2];
		int										_childProcessExitStatus;
		std::chrono::system_clock::time_point	_startTimeChild;
		std::string								_response;

		int		checkTimeoutChild();
		bool	checkIfCgiPipe();

};

#endif
