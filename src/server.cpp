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
	struct	sockaddr_in	servaddr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //AF_INET = internet socket, SOCK_STREAM = tcp stream
		return (write_exit("socket error"));
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
		std::cout << "Waiting for a connection on port " << SERVER_PORT << std::endl;
		connfd = accept(listenfd, (SA *) NULL, NULL); //set to NULL because doesn't matter who connects, just accept
		newReq = new Request(connfd);
		newReq->processReq();
		// newReq->printRequest();
		
		// determine which server should handle this request
			// find the server that corresponds to the request field's Host (?)
			// otherwise give it to the default one
		newResp = new Response(*newReq);
		delete newReq;
		
		response = newResp->createResponse();
		
		if (response)
		{
			// std::cout << "About to return " << newResp->getMsgLength() << "bytes: " << response << std::endl;
			send(connfd, (char*)response, newResp->getMsgLength(), 0);
			delete response;
		}
		delete newResp;

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

