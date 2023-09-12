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
#include <csignal>

#include "Request.hpp"
#include "Response.hpp"
#include <vector>
#include "Connection.hpp"
// #include "PostCGI.hpp"

void	printAllConnections(std::map<int, Connection> conn)
{
	std::cout << "All connection fds: ";
	std::map<int, Connection>::iterator it;
	for (it = conn.begin(); it != conn.end(); it++)
		std::cout << it->first << " ,";
	std::cout << std::endl;
}

/**
 * @brief Checks if the fd that an event took place on matches any socket fd's
 * 
 * @param eventfd the fd the event took place on
 * @return int 
 */

int		Webserver::comparefd(int eventfd)
{
	for (size_t i = 0; i < _sckts.size(); i++)
	{
		if (_sckts.at(i).getListenfd() == eventfd)
			return (i);
	}
	return (-1);
}

int		Webserver::checkIfCgiFd(int evFd)
{
	std::map<int, int>::iterator it;
	for (it = _cgiFds.begin(); it != _cgiFds.end(); it++)
		if (it->first == evFd)
			return (it->second);
	return (evFd);
}

/**
 * @brief Handles and eof event, where the client disconnects from the server
 * 
 * @param connfd 
 * @param ident 
 */

void	Webserver::eofEvent(/*int connfd, */int ident)
{
	std::cout << "Disconnect" << std::endl;
	if (close(ident) < 0)
		throw Webserver::CloseError();
}

// bool	timeout()
// {
// 	long int	time = static_cast<long int>(std::time(NULL));
	//error handling for std::time inbouwen!
	// if (startTime > MAX_DURATION)
	// 	return (true);
// }

/**
 * @brief Starts and runs the loop of the webserver that checks for events
 * 
 * @param servers 
 */

void	Webserver::runWebserver(std::vector<Server> servers)
{
	int nev, connfd;
	struct kevent evList;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);
	int	eventSocket;

	while (1)
	{
		running = true;//weg?
		if ((nev = kevent(_kq, NULL, 0, &evList, 1, NULL)) < 0) //<0 [WAS 1] because the return value is the num of events place in queue
			throw Webserver::KeventError();
		// if (timeout())
		// 	continue ;
		if (evList.flags & EV_EOF)
			eofEvent(evList.ident);
		else if ((eventSocket = comparefd((int)evList.ident)) > -1)
		{
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~Connection accepted~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" << std::endl; //(Used to print Here1 here)
			if ((connfd = accept(evList.ident, (struct sockaddr *)&addr, &socklen)) < 0)
				throw Webserver::AcceptError();
			this->_connections[connfd] = Connection((int)evList.ident, _sckts.at(eventSocket));
			this->_connections[connfd].setRequest(new Request(connfd, _sckts.at(eventSocket).getAddress()));
			if (fcntl(connfd, F_SETFL, O_NONBLOCK) < 0)
			{
				perror("fctnl");
				return ;
			}
			EV_SET(&evList, connfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			if (kevent(_kq, &evList, 1, NULL, 0, NULL) == -1)
				throw Webserver::KeventError();
			// if ((close(connfd)) < 0)
			// 	throw Webserver::CloseError(); //Advice Swaan
			//Als ik dit if-statement uitcomment: pagina blijft laden na "Uploaden succesvol"
		}
		else if (evList.filter == EVFILT_READ)
		{
			int evFd = checkIfCgiFd((int)evList.ident);
			if (_connections[evFd].getResponse())
			{
				if (_connections[evFd].getResponse()->getState() == READ_CGI)
				{
					std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~READ EVENT FOR CGI~~~~~~~~~~~~~~~~~~~~\n\n" << std::endl; //(Used to print Here1 here)
					_connections[evFd].handleResponse();//newReq, newResp, handlingServer);//;connfd, servers); //of moet connfd hier wel evList.ident zijn?
				}
			}
			else
			{
				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~READ EVENT~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" << std::endl; //(Used to print Here1 here)
				// std::cout << "Data size to be read: " << evList.data << std::endl;
				//At each call ofthis event, add a oneshot event for the timeout event (EVFILT_TIMER)!
				_connections[(int)evFd].handleRequest(evFd, servers);//, handlingServer, newReq);
				if (_connections[(int)evFd].getRequest()->getState() == WRITE)
				{
					// std::cout << "~~~~~~~~~~~~~~~~~~WRITE event filter added for conn fd " << (int)evFd << " with state " << _connections[(int)evFd].getRequest()->getState() << "~~~~~~~~~~~~~~~~\n\n" << std::endl;
					EV_SET(&evList, (int)evFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					if (kevent(_kq, &evList, 1, NULL, 0, NULL) == -1)
						throw Webserver::KeventError();
				}
			}

		}
		else if (evList.filter == EVFILT_WRITE)// && _connections[(int)evList.ident].getRequest()->getState() == WRITE)//Hier response senden!
		{
			int evFd = checkIfCgiFd((int)evList.ident);
			std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~WRITE EVENT~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" << std::endl;
			std::cout << " Connection id is [" << (int)evList.ident << "]" << std::endl;
			// printAllConnections(_connections);
			// if (_connections[evFd].getResponse())
			// 	std::cout << "State at start write event: " << _connections[evFd].getResponse()->getState() << std::endl;
			// std::cout << "Space left in writing buffer: " << evList.data << std::endl;

			//send response content that you bind to your request class. When all data is sent, delete TIMEOUT events and close conn
			_connections[evFd].handleResponse();//newReq, newResp, handlingServer);//;connfd, servers); //of moet connfd hier wel evList.ident zijn?
			if (_connections[evFd].getResponse() == nullptr)//In case the Response is sent and finished
			{
				std::cout << "~~~~~~~~~~~~~~~~~~WRITE event filter deleted for conn fd " << evFd << "~~~~~~~~~~~~~~~~\n\n" << std::endl;
				EV_SET(&evList, evFd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
				if (kevent(_kq, &evList, 1, NULL, 0, NULL) == -1)
					throw Webserver::KeventError();
			}
			else if(_connections[evFd].getResponse()->getState() == INIT_CGI && _connections[evFd].getResponse()->cgiOnKqueue == false)
			{
				std::cout << "~~~~~~~~~~~~~~~~~~WRITE event filter added for cgi pipes ~~~~~~~~~~~~~~~~\n\n" << std::endl;
				EV_SET(&evList, _connections[evFd].getResponse()->getCgi().getScriptToWebserv()[0], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				if (kevent(_kq, &evList, 1, NULL, 0, NULL) == -1)
					throw Webserver::KeventError();
				std::cout << " Connection id is [" << evFd << "]" << std::endl;
				std::cout << "WebservToScript[1] = [" << std::endl;
				std::cout << "ScriptToWebserv[0] = " << _connections[evFd].getResponse()->getCgi().getScriptToWebserv()[0] << std::endl;
				std::cout << _connections[evFd].getResponse()->getCgi().getWebservToScript()[1] << "]" << std::endl;
				EV_SET(&evList, _connections[evFd].getResponse()->getCgi().getWebservToScript()[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
				if (kevent(_kq, &evList, 1, NULL, 0, NULL) == -1)
					throw Webserver::KeventError();
				_connections[evFd].getResponse()->cgiOnKqueue = true;
				_connections[evFd].getResponse()->setState(WRITE_CGI);
				_cgiFds.insert({evFd, _connections[evFd].getResponse()->getCgi().getScriptToWebserv()[0]});
				_cgiFds.insert({evFd, _connections[evFd].getResponse()->getCgi().getWebservToScript()[1]});
				std::cout << "PRINT STATEMENT *CHEER*" << std::endl;
			}

		}
		// else if (evList.filter == EVFILT_TIMER)
		// {
		// 	std::cout << "Time-out!!" << std::endl;
		// 	//Hier Timeout handlen: bind current request to a 408 response Error, delete read ev and add write ev
		// }
		// else if (connfd != 0)
		// 	close(connfd);
		
	}
	running = false;
}

void	sighandler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << "\nWebserver will be stopped now, adieu!" << std::endl;
		exit(0); //(Is this correct exit code)
	}
}

/**
 * @brief Sets custom behaviour for signals SIGINT and SiGQUIT
 * 
 */
void	Webserver::setSignal()
{
	std::signal(SIGINT, sighandler); //handle potential error!
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
	setSignal();
	if ((_kq = kqueue()) < 0)
		throw Webserver::KeventError();
	struct kevent evSet;
	for (size_t i = 0; i < servers.size(); i++)
	{
		for (size_t j = 0; j < servers.at(i).getListens().size(); j++)
		{
			Socket sock(servers.at(i).getListens().at(j).first, servers.at(i).getListens().at(j).second, _kq, evSet); // JMA: Can be written slightly shorter: Socket sock(servers.at(i).getHost(j), servers.at(i).getPort(j), kq, evSet);
			_sckts.push_back(sock);
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