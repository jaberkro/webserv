#include "webserver.hpp"
#include "socket.hpp"
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/event.h>
#include <fstream>
#include <fcntl.h>

#include "Request.hpp"
#include "Response.hpp"
#include <vector>

bool	Webserver::write_exit(std::string error)
{
	std::cout << "ERROR: " << error << std::endl;
	return (false);
}

int		Webserver::comparefd(std::vector<Socket> sckts, int eventfd)
{
	for (size_t i = 0; i < sckts.size(); i++)
	{
		if (sckts.at(i).getListenfd() == eventfd)
			return (1);
	}
	return (0);
}

void	Webserver::start(std::vector<Server> servers)
{
	std::vector<Socket> sckts;
	if (this->running)
		return ;//(false);
	for (size_t i = 0; i < servers.size(); i++)
	{
		for (size_t j = 0; j < servers.at(i).getListens().size(); j++)
		{
			Socket sock(servers.at(i).getListens().at(j).second);
			sckts.push_back(sock);
		}
	}
	int kq = kqueue();
	struct kevent evSet;
	for (size_t i = 0; i < sckts.size(); i++)
	{
		sckts.at(i).setUpConn(kq, evSet);
	}
	//watchLoop();
	struct kevent evList[2];
	//EV_LIST(&evList[0], )
	int nev, i;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);
	int fd;
	Request		*newReq;
	Response	*newResp;
	// std::vector<Location>	locations;
	// uint8_t		*response; // needs to be malloced 

	

	// createLocation(locations);

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
			else if (comparefd(sckts, (int)evList[i].ident) == 1)//(int)evList[i].ident == sckt1.listenfd || (int)evList[i].ident == sckt2.listenfd)
			{
				printf("Here1\n");

				fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
				if (fd == -1)
				{
					write_exit("accept error");
					return ;
				}
				// if (connAdd(fd) == 0)
				// {
					if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
					{
						write_exit("fcntl error");
						return ;
					}
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
					newResp->prepareResponseGET(servers.at(0).getLocations()); // argument is ref to the Server
					
					// if (response)
					// {
					// 	// std::cout << "About to return " << newResp->getMsgLength() << "bytes: " << response << std::endl;
					// 	send(fd, (char*)response, newResp->getMsgLength(), 0);
					// 	delete response;
					// }
					delete newResp;
				// uint8_t buff[MAXLINE + 1];
				// 	//snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK \r\nContent-Type: text/html\r\nContent-Length: 20\r\n\r\nWe socket thisssssss"); //can write formatted output to sized buf
				// 	std::string   fileBuf;
				// 	std::string line;
				// 	std::ifstream   htmlFile;
				// 	htmlFile.open("data/www/index.html");
				// 	while (std::getline (htmlFile, line))
				// 		fileBuf += line;
				// 	snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK \r\nContent-Type: text/html\r\nContent-Length: %lu\r\n\r\n%s", fileBuf.length(), fileBuf.c_str());
				// 	write(fd, (char*)buff, std::strlen((char*)buff));
				// 	htmlFile.close();
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

