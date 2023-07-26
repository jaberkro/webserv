#ifndef POSTCGI_HPP
# define POSTCGI_HPP
#include <unistd.h>
#include <iostream>

#define NUM_OF_ARGS 6
#define R 0
#define W 1
#define LEN 200

class PostCGI
{
	private:
		pid_t	id;
		int		webservToScript[2];
		int		scriptToWebserv[2];
		int		exitCode;
		char	**arg;
		size_t	sizeEnv;
		char	**env;//std:string of vector van maken?
		std::string	response;
	public:
		PostCGI();
		~PostCGI();
		void	run(std::string fullRequest);
		std::string	getResponse();
};

#endif