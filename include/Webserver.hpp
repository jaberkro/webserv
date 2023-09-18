#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "Socket.hpp" // check all headers still needed
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Connection.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

class Socket; // why is this here?
class Response;
class CGI;

class Webserver
{
	private:
		bool						_running; //BS:houden of weg?
		std::vector<Socket> 		_sckts;
		std::map<int, Connection>	_connections;
		std::map<int, int>			_cgiFds; //first one is cgi Fd, second one is corresponding connFd
		int							_kq;
		struct kevent				_evList;
		int							comparefd(int fd);
		void						setSignal();
		void						runWebserver(std::vector<Server> servers);
		void						newConnection(int eventSocket, int ident);
		void						eofEvent(int ident, int reason);
		void						readEvent(std::vector<Server> servers);
		void						writeEvent();
		void						addWriteFilter(int fd);
		void						deleteWriteFilter(int fd);
		void						addReadFilter(int fd);
		void						addTimerFilter(int fd);

		Webserver(const Webserver &src); //private because shouldn't be instantiated!
		Webserver& operator=(const Webserver &src); //idem

	public:
		Webserver(std::vector<Server> servers);
		~Webserver();
		int							checkIfCgiFd(int evFd);

	class KeventError : public std::exception {
		public:
			KeventError() : message(std::strerror(errno)) {}
			const char*	what() const throw()
			{
				std::cout << "Kevent error: "; // SERVER COULD NOT START BECAUSE OF: KQUEUE
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
