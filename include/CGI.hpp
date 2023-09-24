#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include "Request.hpp"

#define R 0
#define W 1

class Response;

typedef std::chrono::system_clock::time_point time_point;

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
		pid_t	getId();
		void	cgiRead(Response & response, std::string & fullResponse);
		void	cgiWrite(Response & response);
		void	closePipes(size_t whichPipes);

		std::string	getResponse();

		class CgiError : public std::exception {
			public:
				const char*	what() const throw()
				{
					return ("Fail");
				}
		};

		class CgiScriptTimeout : public std::exception {
			public:
				const char*	what() const throw()
				{
					return ("Timeout");
				}
		};

	private:
		Request		& _req;
		char		**_arg;
		char		**_env;
		pid_t		_id;
		int			_webservToScript[2];
		int			_scriptToWebserv[2];
		int			_childProcessExitStatus;
		time_point	_startTimeChild;
		std::string	_response;

		void	checkTimeoutChild(void);
		void	executeScript(void);
		void	checkChildProcessExitCode(Response & response, \
		std::string & fullResponse);
		void	cleanUp(void);
		void	addToEnv(size_t &i, std::string what);
		char	*protectedStrdup(std::string str);
		char** 	copyEnvironToEnv(char **environ, size_t &i);
};

#endif
