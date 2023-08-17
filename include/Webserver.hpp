#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include "Socket.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Connection.hpp"
#include <map>

class Socket;

# define MAXLINE 60000 //which value should this be?

class Webserver
{
	private:
		bool						running; //BS:houden of weg?
		std::vector<Socket> 		_sckts;
		std::map<int, Connection>	_connections;
		int							_kq;
		int							comparefd(int fd);
		void						setSignal();
		void						runWebserver(std::vector<Server> servers);
		void						eofEvent(int ident);
		Webserver(const Webserver &src); //private because shouldn't be instantiated!
		Webserver& operator=(const Webserver &src); //idem
	public:
		Webserver(std::vector<Server> servers);
		~Webserver();

	class KeventError : public std::exception {
		public:
			KeventError() : message(std::strerror(errno)) {}
			const char*	what() const throw()
			{
				std::cout << "Kevent error: ";
				return (message.c_str());
			}
		private:
			std::string message;
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
