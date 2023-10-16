#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <sys/event.h>
#include <netdb.h>
#include <unistd.h>
#include <string>

# define SA struct sockaddr

class Socket
{
	public:
		Socket(std::string address, unsigned short newport, int kq, struct kevent evSet);
		int					getListenfd() const;
		std::string			getAddress() const;
		unsigned short		getPort() const;

		class SocketError : public std::exception {
			public:
				const char*	what() const throw()
				{
					return ("Creation of socket failed");
				}
		};

		class SetsockoptError : public std::exception {
			public:
				const char*	what() const throw()
				{
					return ("Setsockopt failed");
				}
		};

		class AddressConversionError : public std::exception {
			public:
				const char*	what() const throw()
				{
					return ("Converting address for socket failed");
				}
		};

		class BindError : public std::exception {
			public:
				BindError() : message(std::strerror(errno)) {}
				const char*	what() const throw()
				{
					std::cout << "Bind error: ";
					return (message.c_str());
				}
			private:
				std::string message;
		};

		class ListenError : public std::exception {
			public:
				const char*	what() const throw()
				{
					return ("Listen for socket failed");
				}
		};

		class KeventError : public std::exception {
			public:
				const char*	what() const throw()
				{
					return ("Kevent failed");
				}
		};

	private:
		unsigned short 		_port;
		int					_listenfd;
		std::string			_address;
		struct	sockaddr_in	_servAddr;
		void				setAddressHostPort(std::string address);
};

#endif
