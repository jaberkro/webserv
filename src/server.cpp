#include "../include/server.hpp"
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/event.h>
#include <fstream>

#define NUSERS 10

struct uc { //this should go in the class!!
    int uc_fd;
    char *uc_addr;
} users[NUSERS];


#include <fstream>
#include "Request.hpp"
#include "Response.hpp"
#include <vector>

bool	write_exit(std::string error)
{
	std::cout << "ERROR: " << error << std::endl;
	return (false);
}

/* find the index of a file descriptor or a new slot if fd=0 */
int	conn_index(int fd) 
{
    int uidx;
    for (uidx = 0; uidx < NUSERS; uidx++)
        if (users[uidx].uc_fd == fd)
            return uidx;
    return -1;
}

int conn_add(int fd) 
{
    int uidx;
    if (fd < 1) return -1;
    if ((uidx = conn_index(0)) == -1)
        return -1;
    if (uidx == NUSERS) {
        close(fd);
        return -1;
    }
    users[uidx].uc_fd = fd; /* users file descriptor */
    users[uidx].uc_addr = 0; /* user IP address */
    return 0;
}

/* remove a connection and close it's fd */
int conn_del(int fd) {
    int uidx;
    if (fd < 1) 
		return -1;
    if ((uidx = conn_index(fd)) == -1)
        return -1;

    users[uidx].uc_fd = 0;
    users[uidx].uc_addr = NULL;

    /* free(users[uidx].uc_addr); */
    return close(fd);
}

void	watch_loop(int kq, int listenfd)
{
	struct kevent evSet;
	struct kevent evList[32];
	int nev, i;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);
	int fd;
	Request		*newReq;
	Response	*newResp;
	uint8_t		*response; // needs to be malloced 
	serverBlock	srvBlock;

	locBlock	loc1;
	std::pair<std::string,std::string>	index1("index", "index.html");
	locBlock	loc2;
	std::pair<std::string,std::string>	root2("root", "data/www");
	std::pair<int,std::string>	error2(404, "/404.html");
	locBlock	loc3;
	std::pair<std::string,std::string>	root3("root", "data/images");
	std::pair<int,std::string>	error3(404, "/404.html");

	// filling the dummy location blocks
	loc1.locationModifier = "=";
	loc1.locationMatch = "/";
	loc1.locationDirectives.push_back(index1);
	srvBlock.locations.push_back(loc1);
	loc2.locationModifier = "";
	loc2.locationMatch = "/";
	loc2.locationDirectives.push_back(root2);
	loc2.errorPages.push_back(error2);
	srvBlock.locations.push_back(loc2);
	loc3.locationModifier = "";
	loc3.locationMatch = "/images";
	loc3.locationDirectives.push_back(root3);
	loc3.errorPages.push_back(error3);
	srvBlock.locations.push_back(loc3);

	while (1)
	{
		nev = kevent(kq, NULL, 0, evList, 32, NULL);
		if (nev < 1)
		{
			write_exit("kevent error");
			return ;
		}
		for (i = 0; i<nev; i++)
		{
			if (evList[i].flags & EV_EOF)
			{
				printf("Disconnect\n");
				fd = evList[i].ident;
				EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
				{
					write_exit("kevent error");
					return ;
				}
				//users[]
				conn_del(fd);
				// close(fd);
			}
			else if ((int)evList[i].ident == listenfd)
			{
				// printf("Here1\n");

				fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
				if (fd == -1)
				{
					write_exit("accept error");
					return ;
				}
				if (conn_add(fd) == 0)
				{
					EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
					if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					{
						write_exit("accept error");
						return ;
					}
			// READ AND WRITE ALWAYS USING KQ!
					newReq = new Request(fd);
					newReq->processReq();
					newReq->printRequest();
			// determine which server should handle this request
			// 1. parse "listen" directives, if multiple matches with equal specificity:
			// 2. parse "server name" directives find the server that corresponds to the request field's Host
			// otherwise give it to the default one
					// serverBlockInit(serverBlock);
				// testing whether dummy structure was filled
					// std::cout << "Server block info:" << std::endl;
					// std::cout << serverBlock.locations[0].locationModifier << " " \
					// << serverBlock.locations[0].locationMatch << " " \
					// << serverBlock.locations[0].locationDirectives[0].first << ": " \
					// << serverBlock.locations[0].locationDirectives[0].second << std::endl;
					// std::cout << serverBlock.locations[1].locationModifier << " " \
					// << serverBlock.locations[1].locationMatch << " " \
					// << serverBlock.locations[1].locationDirectives[0].first << ": " \
					// << serverBlock.locations[1].locationDirectives[0].second \
					// << " " << serverBlock.locations[1].errorPages[0].first << ": " \
					// << serverBlock.locations[1].errorPages[0].second << std::endl;
					// std::cout << serverBlock.locations[2].locationModifier << " " \
					// << serverBlock.locations[2].locationMatch << " " \
					// << serverBlock.locations[2].locationDirectives[0].first << ": " \
					// << serverBlock.locations[2].locationDirectives[0].second \
					// << " " << serverBlock.locations[2].errorPages[0].first << ": " \
					// << serverBlock.locations[2].errorPages[0].second << std::endl;
					newResp = new Response(*newReq);
					delete newReq;
					response = newResp->createResponse(srvBlock);
					
					if (response)
					{
						// std::cout << "About to return " << newResp->getMsgLength() << "bytes: " << response << std::endl;
						send(fd, (char*)response, newResp->getMsgLength(), 0);
						delete response;
					}
					delete newResp;
				}
				else
				{
					printf("Connection refused\n");
				}
				close(fd);
			}
			else if (evList[i].filter == EVFILT_READ)
			{
				char buf[256];
				size_t bytes_read;

				bytes_read = recv(evList[i].ident, buf, sizeof(buf), 0);
				if ((int)bytes_read < 0)
					printf("%d bytes read\n", (int)bytes_read);
			}
		}
	}
}

bool	Server::start()
{
	if (this->running)
		return (false);
	int		listenfd, connfd;
	struct	sockaddr_in	servAddr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //AF_INET = internet socket, SOCK_STREAM = tcp stream
		return (write_exit("socket error"));
	//setting up address you're listening on
	std::memset(&servAddr, '\0', sizeof(servAddr));
	servAddr.sin_family		= AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // will respond to anything
	servAddr.sin_port		= htons(SERVER_PORT); //port you're listening on

	if ((bind(listenfd, (SA *) &servAddr, sizeof(servAddr))) < 0)//bind listening socket to address
		return (write_exit("bind error"));
	if ((listen(listenfd, 10)) < 0)
		return (write_exit("listen error"));

	int kq;
	struct kevent evSet;
	kq = kqueue();
	EV_SET(&evSet, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);//EV_SET is a macro that fills the kevent struct
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		return (write_exit("kqueue/kevent error"));
	watch_loop(kq, listenfd);
	return (true);	
}



// void	Server::server()
// {
// 	Server	server = new Server();

// 	server.start(ipAddress, port);
// 	server.stop();
// }
