#include "../include/server.hpp"
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include "Request.hpp"
#include "Response.hpp"
#include <vector>

bool	write_exit(std::string error)
{
	std::cout << "ERROR: " << error << std::endl;
	return (false);
}

bool	Server::start()
{
	Request		*newReq;
	Response	*newResp;
	uint8_t		*response; // needs to be malloced 
	if (this->running)
		return (false);
	int		listenfd, connfd;
	// int	n; //connfd will actually talk to the client that's connected
	struct	sockaddr_in	servaddr;
	// uint8_t buff[MAXLINE + 1];
	// uint8_t recvline[MAXLINE + 1];

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
		// Request	newReq(connfd);
		newReq = new Request(connfd);
		newReq->processReq();
		// allReq[0].printRequest();
		// allReq.push_back(newReq);
		// std::cout  << &newReq << ", allReq[last]: " << &allReq[allReq.size() - 1] << std::endl;
		std::cout << "newReq: " << newReq->getTarget() << std::endl;
		// std::cout << "allReq[0]: " << allReq[0].getTarget() << std::endl;
		// std::cout << "allReq[last]: " << allReq[allReq.size() - 1].getTarget() << std::endl;
		newResp = new Response(*newReq);
		delete newReq;
		// allReq.pop_front();
		// allResp.push_back(newResp);
		newResp->createResponse(&response);
		delete newResp;
		// std::memset(recvline, 0, MAXLINE);
		// while ((n = read(connfd, recvline, MAXLINE - 1)) > 0)
		// {
		// 	std::cout << "\n" << recvline << std::endl;
		// 	if (recvline[n - 1] == '\n')
		// 		break ;
		// }
		// [Darina:] here do (http) request parsing & if necessary send stuff to CGI

		// std::memset(recvline, 0, MAXLINE);
		// if (n < 0) //can't read negative bytes
		// 	return (write_exit("read error"));
		
		// std::string	fileBuf;
		// std::string	line;
		// std::ifstream	htmlFile;
		// htmlFile.open("data/www/index.html");
		// while (std::getline (htmlFile, line))
		// 	fileBuf += line;
		// snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK \r\nContent-Type: text/html\r\nContent-Length: %lu\r\n\r\n%s", fileBuf.length(), fileBuf.c_str()); //can write formatted output to sized buf
		// // snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK \r\n\r\nWe socket this"); //can write formatted output to sized buf
		std::cout << "About to return " << response << std::endl;
		write(connfd, (char*)response, std::strlen((char*)response));
		// std::memset(buff, 0, MAXLINE);
		// fileBuf.clear();
		// std::ifstream	imgFile;
		// imgFile.open("data/images/cat.jpg");
		// while (std::getline (imgFile, line))
		// 	fileBuf += line;
		// snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK \r\nContent-Type: image/jpg\r\nContent-Lenght: 75447\r\n\r\n%s", fileBuf.c_str()); //can write formatted output to sized buf
		// write(connfd, (char*)buff, std::strlen((char*)buff));
		
		// htmlFile.close();
		// imgFile.close();
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

