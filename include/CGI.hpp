#ifndef CGI_HPP
# define CGI_HPP

#include <unistd.h>
#include <iostream>
#include "Request.hpp"
// #include "Response.hpp"


#define R 0
#define W 1

class Response;

class CGI
{
	private:
		Request		& _req;
		char		**_arg;
		char		**_env;//std:string of vector van maken? [Darina: denk dat het zo kan blijven want execve verwacht een **char]
		pid_t		id;
		int			_webservToScript[2];
		int			_scriptToWebserv[2];
		int			_childProcessExitStatus;
		std::string	_response;
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
		void	cgiRead(Response & response, std::string & fullResponse);
		void	cgiWrite(Response & response);



		std::string	getResponse();
};

#endif