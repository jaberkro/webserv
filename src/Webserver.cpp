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
/**
 * @brief Starts and runs the loop of the webserver that checks for events
 * 
 * @param evSet 
 * @param servers 
 */

void	Webserver::runWebserver(std::vector<Server> servers)
{
	int fd, nev, i;
	struct kevent evList;
	Request		*newReq;
	Response	*newResp;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);

	while (1)
	{
		running = true;
		if ((nev = kevent(kq, NULL, 0, &evList, 2, NULL)) < 1) //<1 because the return value is the num of events place in queue
			throw Webserver::KeventError();
		for (i = 0; i<nev; i++)
		{
			if (evList.flags & EV_EOF)
			{
				printf("Disconnect\n");
				fd = evList.ident;
				EV_SET(&evList, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);//was evSet
				if (kevent(kq, &evList, 1, NULL, 0, NULL) == -1)//was evSet
					throw Webserver::KeventError();
				close(fd);
			}
			else if (comparefd((int)evList.ident) == 1)
			{
				printf("Here1\n");

				if ((fd = accept(evList.ident, (struct sockaddr *)&addr, &socklen)) < 0)
					throw Webserver::AcceptError();
				// if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
				// {
				// 	write_exit("fcntl error");
				// 	return ;
				// }
				EV_SET(&evList, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);//was evSet
				if (kevent(kq, &evList, 1, NULL, 0, NULL) == -1)//was evSet
					throw Webserver::KeventError();
				try
				{
					newReq = new Request(fd);
					newReq->processReq();
					newReq->printRequest();
					Server const &	handler = newReq->identifyServer(servers);
					std::cout << "Responsible server is " << \
					handler.getServerName(0) << std::endl;
					newResp = new Response(*newReq);
					if (newReq->getMethod() == "POST")
					{
						newReq = new Request(fd);
						newReq->processReq();
						newReq->printRequest();
						Server const &	handler = newReq->identifyServer(servers);
						std::cout << "Responsible server is " << \
						handler.getServerName(0) << std::endl;
						newResp = new Response(*newReq);
						if (newReq->getMethod() == "POST")
						{
								std::cout << "\n\nFULLREQUEST: [" << newReq->getFullRequest() << "]" << std::endl;
							//Hier werkt request value nog
							newResp->prepareResponsePOST(handler);
							delete newReq;
						}
						else
						{
							delete newReq;
							newResp->prepareResponseGET(handler);
						}
						delete newResp;
					}
					// else if (newReq->getMethod() == "DELETE")
					// {
					// 	std::cout << "Let's gooooooo delete!!" << std::endl;
					// 	newResp->prepareResponseDELETE(handler, newReq->getFullRequest());
					// 	delete newReq;
					// }
					else
					{
						delete newReq;
						newResp->prepareResponseGET(handler);
					}
					delete newResp;
				}
				catch(const std::exception& e)
				{
					std::cerr << "!!! " << e.what() << '\n';
				}
				if ((close(fd)) < 0)
					throw Webserver::CloseError();
			}
			else if (evList.filter == EVFILT_READ)
			{
				char buf[256];
				size_t bytes_read;

				bytes_read = recv(evList.ident, buf, sizeof(buf), 0);
				printf("=================%d BYTESSSSSSSS===============\n", (int)bytes_read);
				if ((int)bytes_read < 0)
					printf("%d bytes read\n", (int)bytes_read);
			}
		}
	}
	running = false;
}

/**
 * @brief Construct a new Webserver:: Webserver object.
 * I) the kqueue will be initialized
 * II) the sockets will be initialized
 * II) the loop to check the connections and events is started with runWebserver()
 * 
 * @param servers a vector of Server instances with information about every server
 */

Webserver::Webserver(std::vector<Server> servers)
{
	if ((kq = kqueue()) < 0)
		throw Webserver::KeventError();
	struct kevent evSet;
	for (size_t i = 0; i < servers.size(); i++)
	{
		for (size_t j = 0; j < servers.at(i).getListens().size(); j++)
		{
			Socket sock(servers.at(i).getListens().at(j).first, servers.at(i).getListens().at(j).second, kq, evSet); // JMA: Can be written slightly shorter: Socket sock(servers.at(i).getHost(j), servers.at(i).getPort(j), kq, evSet);
			sckts.push_back(sock);
		}
	}
	runWebserver(servers);
}

// Webserver::Webserver(const Webserver &src)
// {

// }

// Webserver::Webserver& operator=(const Webserver &src)
// {
// 	running = src.running;

// }


Webserver::~Webserver()
{
	//Hier nog correcte fd's sluiten!
}