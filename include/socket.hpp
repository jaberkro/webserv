#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/event.h>
#include <unistd.h>
#include <arpa/inet.h>


class Socket
{
	private:
		unsigned int port;
		// struct uc {
		// 	int uc_fd;
		// 	char *uc_addr;
		// } users[10]; //10 is hardcoded!!
		// ^^ ?? if I use this instead of global, it fails!? ^^
		struct kevent evSet;
		int kq;
		struct	sockaddr_in	servAddr;
		int	listenfd;
		void	watchLoop();
		int		connIndex(int fd);
		int		connAdd(int fd);
		int		connDelete(int fd);
	public:
		Socket(unsigned short newport);
		bool	setUpConn();
};

#endif
