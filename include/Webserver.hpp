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
		bool						_running;
		std::vector<Socket> 		_sckts;
		std::map<int, Connection>	_connections;
		std::map<int, int>			_cgiFds; //First one in pair is cgi fd, second one is corresponding connFd
		int							_kq;
		struct kevent				_evList;
		int							comparefd(int fd);
		void						setSignal();
		void						runWebserver(std::vector<Server> servers);
		void						newConnection(int eventSocket, int ident);
		void						eofEvent(int ident);
		void						readEvent(std::vector<Server> servers);
		void						writeEvent();
		void						addWriteFilter(int fd);
		void						deleteWriteFilter(int fd);
		void						addReadFilter(int fd);
		void						addTimerFilter(int fd);

		Webserver(const Webserver &src);
		Webserver& operator=(const Webserver &src);

	public:
		Webserver(std::vector<Server> servers);
		~Webserver();
		int							checkIfCgiFd(int evFd);

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
	public:
		serverBlock() {};
		~serverBlock() {};

		std::string										serverName;
		std::vector<std::pair<std::string,std::string>>	serverDirectives;
};

// dummy functions
void	serverBlockInit(serverBlock & sb);

#endif
