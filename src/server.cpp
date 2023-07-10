#include "webserver.hpp"
#include "socket.hpp"
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
int	Socket::connIndex(int fd) 
{
    int uidx;
    for (uidx = 0; uidx < NUSERS; uidx++)
        if (users[uidx].uc_fd == fd)
            return uidx;
    return -1;
}

int Socket::connAdd(int fd) 
{
    int uidx;
    if (fd < 1) return -1;
    if ((uidx = connIndex(0)) == -1)
        return -1;
    if (uidx == NUSERS) {
        close(fd);
        return -1;
    }
    users[uidx].uc_fd = fd; /* users file descriptor */
    users[uidx].uc_addr = 0; /* user IP address */
    return 0;
}

/* remove a connection and close its fd */
int Socket::connDelete(int fd) {
    int uidx;
    if (fd < 1) 
		return -1;
    if ((uidx = connIndex(fd)) == -1)
        return -1;

    users[uidx].uc_fd = 0;
    users[uidx].uc_addr = NULL;

    /* free(users[uidx].uc_addr); */
    return close(fd);
}

// void	Socket::watchLoop()
// {
	// struct kevent evList[32];
	// int nev, i;
	// struct sockaddr_storage addr;
	// socklen_t socklen = sizeof(addr);
	// int fd;
	// while (1)
	// {
	// 	nev = kevent(kq, NULL, 0, evList, 32, NULL);
	// 	if (nev < 1)
	// 	{
	// 		write_exit("kevent error");
	// 		return ;
	// 	}
	// 	for (i = 0; i<nev; i++)
	// 	{
	// 		if (evList[i].flags & EV_EOF)
	// 		{
	// 			printf("Disconnect\n");
	// 			fd = evList[i].ident;
	// 			EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	// 			if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
	// 			{
	// 				write_exit("kevent error");
	// 				return ;
	// 			}
	// 			connDelete(fd);
	// 		}
	// 		else if ((int)evList[i].ident == listenfd)
	// 		{
	// 			printf("Here1\n");

	// 			fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
	// 			if (fd == -1)
	// 			{
	// 				write_exit("accept error");
	// 				return ;
	// 			}
	// 			if (connAdd(fd) == 0)
	// 			{
	// 				EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	// 				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
	// 				{
	// 					write_exit("accept error");
	// 					return ;
	// 				}
	// 				uint8_t buff[MAXLINE + 1];
	// 				//snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK \r\nContent-Type: text/html\r\nContent-Length: 20\r\n\r\nWe socket thisssssss"); //can write formatted output to sized buf
	// 				std::string   fileBuf;
	// 				std::string line;
	// 				std::ifstream   htmlFile;
	// 				htmlFile.open("data/index.html");
	// 				while (std::getline (htmlFile, line))
	// 					fileBuf += line;
	// 				snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK \r\nContent-Type: text/html\r\nContent-Length: %lu\r\n\r\n%s", fileBuf.length(), fileBuf.c_str());
	// 				write(fd, (char*)buff, std::strlen((char*)buff));
	// 				htmlFile.close();
	// 			}
	// 			else
	// 			{
	// 				printf("Connection refused\n");
	// 				close(fd);
	// 			}
	// 		}
	// 		else if (evList[i].filter == EVFILT_READ)
	// 		{
	// 			char buf[256];
	// 			size_t bytes_read;

	// 			bytes_read = recv(evList[i].ident, buf, sizeof(buf), 0);
	// 			if ((int)bytes_read < 0)
	// 				printf("%d bytes read\n", (int)bytes_read);
	// 		}
	// 	}
	// }
// }

bool	Socket::setUpConn(int kq, struct kevent evSet)
{
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //AF_INET = internet socket, SOCK_STREAM = tcp stream
		return (write_exit("socket error"));
	int reuse; //this and setsockopt avoids the bind error and allows to reuse the address
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
 		return(write_exit("reuse port error"));
	//setting up address you're listening on
	std::memset(&servAddr, '\0', sizeof(servAddr));
	servAddr.sin_family		= AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // will respond to anything
	printf("port: %d\n", this->port);
	servAddr.sin_port		= htons(/*SERVER_PORT*/port); //port you're listening on
	if ((bind(listenfd, (SA *) &servAddr, sizeof(servAddr))) < 0)//bind listening socket to address
		return (write_exit("bind error"));
	if ((listen(listenfd, 10)) < 0)
		return (write_exit("listen error"));
	// kq = kqueue();
	EV_SET(&evSet, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);//EV_SET is a macro that fills the kevent struct
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		return (write_exit("kqueue/kevent error"));
	// watchLoop();
	return (true);	

}

void	Webserver::start()//std::vector<Server> servers)
{
	std::vector<Socket> sckts;
	if (this->running)
		return ;//(false);
	// for (size_t i = 0; i < servers.size(); i++)
	// {
	// 	Socket sock(servers.at(i).getPort(i));
	// 	sckts.push_back(sock);
	// }
	Socket	sckt1(8080);
	Socket	sckt2(4242);
	int kq = kqueue();
	struct kevent evSet;
	sckt1.setUpConn(kq, evSet);
	sckt2.setUpConn(kq, evSet);

	//watchLoop();
	struct kevent evList[2];
	int nev, i;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);
	int fd;
	Request		*newReq;
	Response	*newResp;
	// uint8_t		*response; // needs to be malloced 

	

	

	while (1)
	{
		nev = kevent(kq, NULL, 0, evList, 2, NULL);
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
				//connDelete(fd);
				close(fd);
			}
			else if ((int)evList[i].ident == sckt1.listenfd || (int)evList[i].ident == sckt2.listenfd)//((int)evList[i].ident == (sckt1.listenfd || sckt2.listenfd))
			{
				// printf("Here1\n");

				fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
				if (fd == -1)
				{
					write_exit("accept error");
					return ;
				}
				// if (connAdd(fd) == 0)
				// {
					EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
					if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					{
						write_exit("accept error");
						return ;
					}
					newReq = new Request(fd);
					newReq->processReq();
					newReq->printRequest();
			// determine which server should handle this request (Request::identifyServer)
			// 1. parse "listen" directives, if multiple matches with equal specificity:
			// 2. parse "server name" directives find the server that corresponds to the request field's Host
			// otherwise give it to the default one
			
					newResp = new Response(*newReq);
					delete newReq;
					newResp->createResponse(); // argument is ref to the Server
					
					// if (response)
					// {
					// 	// std::cout << "About to return " << newResp->getMsgLength() << "bytes: " << response << std::endl;
					// 	send(fd, (char*)response, newResp->getMsgLength(), 0);
					// 	delete response;
					// }
					delete newResp;
			// }
			// else
			// {
			// 	printf("Connection refused\n");
			// }
			// close(fd);
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


	// return(sckt.setUpConn());

