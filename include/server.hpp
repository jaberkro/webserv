#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <algorithm>
# include <vector>
# include "socket.hpp"

class Socket;

# define SA struct sockaddr
# define SERVER_PORT 80
# define MAXLINE 4000

class Server
{
	private:
		std::string	contentPath;
		

	public:
		bool		running;
		bool		start();

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
