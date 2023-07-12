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
		unsigned short port;
		// struct uc {
		// 	int uc_fd;
		// 	char *uc_addr;
		// } users[10]; //10 is hardcoded!!
		// ^^ ?? if I use this instead of global, it fails!? ^^
		// struct kevent evSet;
		// int kq;
		int	listenfd;
		struct	sockaddr_in	servAddr;
		int		connIndex(int fd);
		int		connAdd(int fd);
		int		connDelete(int fd);
	public:
		Socket(unsigned short newport);
		int		getListenfd();
		void	watchLoop(); //deze weer implementeren en private maken!
		bool	setUpConn(int kq, struct kevent evSet);
		bool		write_exit(std::string error);

};

#endif
