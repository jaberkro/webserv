#ifndef POSTCGI_HPP
# define POSTCGI_HPP

#include <unistd.h>
#include <iostream>
#include "Request.hpp"
// #include "Response.hpp"


#define R 0
#define W 1

class Response;

class PostCGI
{
	private:
		Request		& _req;
		char		**_arg;
		char		**_env;//std:string of vector van maken? [Darina: denk dat het zo kan blijven want execve verwacht een **char]
		pid_t		id;
		int			_webservToScript[2];
		int			_scriptToWebserv[2];
		int			_exitCode;
		std::string	_response;
	public:
		PostCGI(Request & req);
		~PostCGI();
		PostCGI &	operator=(PostCGI & r);

		void	run(Response & response);
		void	prepareArg(std::string const & scriptName);
		void	prepareEnv(std::string const & scriptName);
		int*	getWebservToScript();
		int*	getScriptToWebserv();
		bool	checkIfCgiPipe();
		void	cgiRead(Response & response);
		void	cgiWrite(Response & response);



		std::string	getResponse();
};

#endif