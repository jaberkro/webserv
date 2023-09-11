#ifndef CGI_HPP
# define CGI_HPP

#include <unistd.h>
#include <iostream>
#include "Request.hpp"
#include "Response.hpp"


#define R 0
#define W 1

class CGI
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
		CGI(Request & req);
		~CGI();
		void	run(Response & response, std::string & fullResponse);
		void	prepareArg(std::string const & scriptName);
		void	prepareEnv(std::string const & scriptName, std::string const & pathInfo);

		std::string	getResponse();
};

#endif