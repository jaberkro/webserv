#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

class Socket
{
	private:
		unsigned short port;
		int	listenfd;
		struct	sockaddr_in	servAddr;
		void	setAddressHostPort(std::string address);
	public:
		Socket(std::string address, unsigned short newport, int kq, struct kevent evSet);
		int		getListenfd();
		void	watchLoop(); //deze weer implementeren en private maken!
		
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
			const char*	what() const throw()
			{
				return ("Binding socket failed");
			}
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
};



#endif
