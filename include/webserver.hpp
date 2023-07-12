#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include "socket.hpp"
#include "Server.hpp"

class Socket;

# define SA struct sockaddr
# define SERVER_PORT 80
# define MAXLINE 4000

class Webserver
{
	private:
		std::string	contentPath;
		

	public:
		bool		running;
		void		start(std::vector<Server> servers);
		int			comparefd(std::vector<Socket> sckts, int fd);
		bool		write_exit(std::string error);


};


class serverBlock
{
	private:
		/* data */
	public:
		serverBlock() {};
		~serverBlock() {};

		std::string										serverName;
		std::vector<std::pair<std::string,std::string>>	serverDirectives;
		// std::vector<Location>							locations;
};

// dummy functions
void	serverBlockInit(serverBlock & sb);


#endif
