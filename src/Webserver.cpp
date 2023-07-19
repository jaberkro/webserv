#include "Webserver.hpp"
#include "Socket.hpp"
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

int		Webserver::comparefd(int eventfd)
{
	for (size_t i = 0; i < sckts.size(); i++)
	{
		if (sckts.at(i).getListenfd() == eventfd)
			return (1);
	}
	return (0);
}

void	Webserver::startLoop(struct kevent evSet, std::vector<Server> servers)
{
	int fd = 0, nev = 0, i;
	struct kevent evList[2];
	Request		*newReq;
	Response	*newResp;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);
	// std::vector<Location>	locations;
	// uint8_t		*response; // needs to be malloced 

	// createLocation(locations);

	while (1)
	{
		running = true;
		if ((nev = kevent(kq, NULL, 0, evList, 2, NULL)) < 1) //<1 because the return value is the num of events place in queue
			throw Webserver::KeventError();
		for (i = 0; i<nev; i++)
		{
			if (evList[i].flags & EV_EOF)
			{
				printf("Disconnect\n");
				fd = evList[i].ident;
				EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				EV_SET(&evSet, fd, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);//see gphilipp Slack
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					throw Webserver::KeventError();
				close(fd);
			}
			else if (comparefd((int)evList[i].ident) == 1)
			{
				printf("Here1\n");

				if ((fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen)) < 0)
					throw Webserver::AcceptError();
				// if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
				// {
				// 	perror("fctnl");
				// 	return ;
				// }
				EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					throw Webserver::KeventError();
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
				if ((close(fd)) < 0)
					throw Webserver::CloseError();
			}
			else if (evList[i].filter == EVFILT_READ) //Hier request readen!! Voor pending content
			{
				//At each call ofthis event, add a oneshot event for the timeout event (EVFILT_TIMER)!
				char buf[evList[i].data]; //eventList.data returns size of pending content
				size_t bytes_read;

				bytes_read = recv(evList[i].ident, buf, sizeof(buf), 0);
				printf("=================%d BYTESSSSSSSS===============\n", (int)bytes_read);
				if ((int)bytes_read < 0)
					printf("%d bytes read\n", (int)bytes_read);
			}
			else if (evList[i].filter == EVFILT_WRITE)//Hier response senden!
			{
				//send response content that you bind to your request class. When all data is sent, delete TIMEOUT events and close conn
				send(evList[i].ident, "Write Event", 11, 0);//This obviously should be smth else
			}
			else if (evList[i].filter == EVFILT_TIMER)
			{
				std::cout << "Time-out!!" << std::endl;
				//Hier Timeout handlen: bind current request to a 408 response Error, delete read ev and add write ev
			}
			else if (fd != 0)
				close(fd);
		}
	}
	running = false;
}

Webserver::Webserver(std::vector<Server> servers)
{
	if (this->running)
		return ;
	if ((kq = kqueue()) < 0)
		throw Webserver::KeventError();
	struct kevent evSet;
	for (size_t i = 0; i < servers.size(); i++)
	{
		for (size_t j = 0; j < servers.at(i).getListens().size(); j++)
		{
			Socket sock(servers.at(i).getListens().at(j).first, servers.at(i).getListens().at(j).second, kq, evSet);
			sckts.push_back(sock);
		}
	}
	startLoop(evSet, servers);
}

