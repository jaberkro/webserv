#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <algorithm>
# include <vector>
# include "socket.hpp"

class Socket;

# define SA struct sockaddr
# define SERVER_PORT 80
# define MAXLINE 80000

class Server
{
	private:
		//Socket		sckt;
		std::string	contentPath;
		

	public:
		bool		running;
		bool		start();

};

typedef struct locationBlock
{
	std::string										locationMatch;
	std::string										locationModifier;
	std::vector<std::pair<std::string,std::string>>	locationDirectives;
	std::vector<std::pair<int,std::string>>			errorPages;
	std::vector<std::string>						tryFiles;
}				locBlock;


class serverBlock
{
	private:
		/* data */
	public:
		serverBlock() {};
		~serverBlock() {};

		std::string										serverName;
		std::vector<std::pair<std::string,std::string>>	serverDirectives;
		std::vector<locBlock>							locations;
};

// dummy functions
void	serverBlockInit(serverBlock & sb);


#endif
