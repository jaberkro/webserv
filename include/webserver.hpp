#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include "Socket.hpp"
#include "Server.hpp"

class Socket;

# define SA struct sockaddr
# define MAXLINE 4000

class Webserver
{
	private:
		bool		running;
		std::vector<Socket> sckts;
		int			kq;
		int			comparefd(int fd);
		void		startLoop(struct kevent evSet, std::vector<Server> servers);
	public:
		Webserver(std::vector<Server> servers);
	class KeventError : public std::exception {
		public:
			const char*	what() const throw()
			{
				return ("Kevent failed");
			}
		};
	class AcceptError : public std::exception {
		public:
			const char*	what() const throw()
			{
				return ("Accept failed");
			}
		};
	class CloseError : public std::exception {
		public:
			const char*	what() const throw()
			{
				return ("Closing fd failed");
			}
		};

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
