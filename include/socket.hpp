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
		//struct	sockaddr_in	servAddr;
		int	listenfd;
		int kq;
		struct kevent evSet;
		unsigned int port;
		struct uc {
			int uc_fd;
			char *uc_addr;
		} users[10]; //10 is hardcoded!!
	public:
		Socket(unsigned short newport);
		bool	setUpConn();
		void	watchLoop();
		int		connIndex(int fd);
		int		connAdd(int fd);
		int		connDelete(int fd);
};

#endif
