#ifndef POSTCGI_HPP
# define POSTCGI_HPP

#include <unistd.h>
#include <iostream>
#include "Request.hpp"

#define R 0
#define W 1
#define LEN 200

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
		void	run();
		void	prepareArg();
		void	prepareEnv();

		std::string	getResponse();
};

#endif