#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include "Socket.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"

class Socket;

# define SA struct sockaddr
# define MAXLINE 4000 //which value should this be?

class Webserver
{
	private:
		bool		running;
		// Request		*newReq;
		// Response	*newResp;
		// Server		*handler;

		std::vector<Socket> sckts;
		int			kq;
		int			comparefd(int fd);
		void		setSignal();
		void		runWebserver(std::vector<Server> servers);
		void		eofEvent(int connfd, int ident);
		void		handleRequest(int connfd, std::vector<Server> servers, Server *& handler, Request *& newReq);
		void		handleResponse(Request *& newReq, Response *& newResp, Server *& handler);//int connfd, std::vector<Server> servers)
		Webserver(const Webserver &src); //private because shouldn't be instantiated!
		Webserver& operator=(const Webserver &src); //idem
	public:
		Webserver(std::vector<Server> servers);
		~Webserver();

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
