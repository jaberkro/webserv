#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include "socket.hpp"
#include "Server.hpp"

class Socket;

#define SA struct sockaddr
#define MAXLINE 4096
#define SERVER_PORT 80

class Webserver
{
	private:
		std::string	contentPath;
	public:
		bool		running;
		void		start(std::vector<Server> servers);
		int			comparefd(std::vector<Socket> sckts, int fd);

};

#endif
