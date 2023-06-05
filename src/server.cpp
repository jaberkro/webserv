#include "../include/server.hpp"
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

bool	write_exit(std::string error)
{
	std::cout << "ERROR: " << error << std::endl;
	return (false);
}

bool	Server::start()
{
	if (this->running)
		return (false);
	int		listenfd, connfd, n; //connfd will actually talk to the client that's connected
	struct	sockaddr_in	servaddr;
	uint8_t buff[MAXLINE + 1];
	uint8_t recvline[MAXLINE + 1];

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //AF_INET = internet socket, SOCK_STREAM = tcp stream
		return (write_exit("socket error"));
	//setting up address you're listening on
	std::memset(&servaddr, '\0', sizeof(servaddr));
	servaddr.sin_family		= AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // will respond to anything
	servaddr.sin_port		= htons(SERVER_PORT); //port you're listening on

	if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0)//bind listening socket to address
		return (write_exit("bind error"));
	if ((listen(listenfd, 10)) < 0)
		return (write_exit("listen error"));

	while(1)
	{
		// struct sockaddr_in	addr;
		// socklen_t	addr_len;
		//accept blocks until an incoming connection arrives. returns an fd to the connection
		std::cout << "Waiting for a connection on port " << SERVER_PORT << std::endl;
		connfd = accept(listenfd, (SA *) NULL, NULL); //set to NULL because doesn't matter who connects, just accept
		std::memset(recvline, 0, MAXLINE);
		while ((n = read(connfd, recvline, MAXLINE - 1)) > 0)
		{
			std::cout << "\n" << recvline << std::endl;
			if (recvline[n - 1] == '\n')
				break ;
		}
		std::memset(recvline, 0, MAXLINE);
		if (n < 0) //can't read negative bytes
			return (write_exit("read error"));
		snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK \r\n\r\nWe socket this"); //can write formatted output to sized buf
		write(connfd, (char*)buff, std::strlen((char*)buff));
		close(connfd);
	}
	return (true);	
}

// void	Server::server()
// {
// 	Server	server = new Server();

// 	server.start(ipAddress, port);
// 	server.stop();
// }

