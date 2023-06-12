#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>

#define SA struct sockaddr
#define SERVER_PORT 80
#define MAXLINE 80000

class Server
{
	private:
		//Socket		serverSocket;
		std::string	contentPath;
	public:
		bool		running;
		bool		start();

};

#endif
