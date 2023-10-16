#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "Connection.hpp"

class Webserver
{
	public:
		Webserver(std::vector<Server> servers);
		~Webserver();

		int	checkIfCgiFd(int evFd);

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
};

#endif
