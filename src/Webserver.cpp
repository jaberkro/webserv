#include "Webserver.hpp"
#include "Socket.hpp"
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/event.h>
#include <sys/time.h> //for kqueue timer
#include <fstream>
#include <fcntl.h>
#include <csignal>
#include <vector>

#include "Request.hpp"
#include "Response.hpp"
#include "Connection.hpp"

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

void	Webserver::addTimerFilter(int fd)
{
	struct timespec timeout;
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;
	EV_SET(&_evList, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_ONESHOT, \
		0, 5000, &timeout);
	if (kevent(_kq, &_evList, 1, NULL, 0, NULL) == -1)
		throw Webserver::KeventError();
	_connections[fd].setTimer(true);
	// std::cout << "Timer set for fd " << fd << std::endl;
}

void	Webserver::addWriteFilter(int evFd)
{
	EV_SET(&_evList, (int)evFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(_kq, &_evList, 1, NULL, 0, NULL) == -1)
		throw Webserver::KeventError();
}

void	Webserver::deleteWriteFilter(int fd)
{
	EV_SET(&_evList, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	if (kevent(_kq, &_evList, 1, NULL, 0, NULL) == -1)
		throw Webserver::KeventError();
}

void	Webserver::addReadFilter(int fd)
{
	EV_SET(&_evList, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(_kq, &_evList, 1, NULL, 0, NULL) == -1)
		throw Webserver::KeventError(); // NO EXCEPTION, STATUSCODE = INTERNAL_SERVER_ERROR + STATE = ERROR, set Write event!
}

/**
 * @brief Handles and eof event, where the client disconnects from the server
 * 
 * @param connfd 
 * @param ident 
 */

void	Webserver::eofEvent(int ident)
{
	if (fcntl(ident, F_GETFD) > -1)
	{
		std::cout << "Disconnect for fd " << ident;
		_connections[ident].setTimer(false);
		std::cout << std::endl;
			if (close(ident) < 0)
				throw Webserver::CloseError();  // NO EXCEPTION, STATUSCODE = INTERNAL_SERVER_ERROR + STATE = ERROR
	}
}

void	Webserver::newConnection(int eventSocket, int ident)
{
	int connfd;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);

	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~Connection accepted~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" << std::endl;
	if ((connfd = accept(ident, (struct sockaddr *)&addr, &socklen)) < 0)
		throw Webserver::AcceptError(); // NO EXCEPTION, STATUSCODE = INTERNAL_SERVER_ERROR + STATE = ERROR
	this->_connections[connfd] = Connection(ident, _sckts.at(eventSocket)); // ADD STATUSCODE AS PARAMETER
	this->_connections[connfd].setRequest(new Request(connfd, _sckts.at(eventSocket).getAddress()));
	////////Starting from here, request state can be error!!//////////
	if (fcntl(connfd, F_SETFL, O_NONBLOCK) < 0)
		_connections[connfd].getRequest()->setError(500);
	addReadFilter(connfd);
}

void	Webserver::readEvent(std::vector<Server> servers)
{
	int evFd = checkIfCgiFd((int)_evList.ident);
	if (_connections[evFd].getResponse())
	{
		if (_connections[evFd].getResponse()->getState() == READ_CGI)
			_connections[evFd].handleResponse();
	}
	else
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~READ EVENT~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" << std::endl;
		_connections[(int)evFd].handleRequest(evFd, servers, _evList.data);
		if (_connections[(int)evFd].getRequest()->getState() == WRITE || _connections[(int)evFd].getRequest()->getState() == REQ_ERROR)
			addWriteFilter(evFd);
	}
}

void	Webserver::writeEvent()
{
	int evFd = checkIfCgiFd((int)_evList.ident);
	// std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~WRITE EVENT~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" << std::endl;
	// std::cout << "Connection id is [" << (int)_evList.ident << "]" << std::endl;
	_connections[evFd].handleResponse();
	if (_connections[evFd].getResponse() == nullptr)//In case the Response is sent and finished, the write filter can be deleted
		deleteWriteFilter(evFd);
	else if (_connections[evFd].getResponse()->getState() == INIT_CGI && _connections[evFd].getResponse()->cgiOnKqueue == false)
	{
		// std::cout << "\n\n~~~~~~~~~~~~~~~~~~WRITE event filter added for cgi pipes ~~~~~~~~~~~~~~~~" << std::endl;
		addWriteFilter(_connections[evFd].getResponse()->getCgi().getScriptToWebserv()[0]);
		addWriteFilter(_connections[evFd].getResponse()->getCgi().getWebservToScript()[1]);
		_connections[evFd].getResponse()->cgiOnKqueue = true;
		_connections[evFd].getResponse()->setState(WRITE_CGI);
		_cgiFds.insert({evFd, _connections[evFd].getResponse()->getCgi().getScriptToWebserv()[0]});
		_cgiFds.insert({evFd, _connections[evFd].getResponse()->getCgi().getWebservToScript()[1]});
	}
	addTimerFilter(evFd);
}

/**
 * @brief Starts and runs the loop of the webserver that checks for events
 * 
 * @param servers 
 */

void	Webserver::runWebserver(std::vector<Server> servers)
{
	int nev;
	int	eventSocket;

	while (1)
	{
		struct timespec *timeout = NULL;
		_running = true;
		if ((nev = kevent(_kq, NULL, 0, &_evList, 1, timeout)) < 0)
			continue ;
		//	throw Webserver::KeventError();//internal server error sturen
		if (_evList.filter == EVFILT_TIMER || _evList.flags & EV_EOF)
			eofEvent(_evList.ident);
		else if ((eventSocket = comparefd((int)_evList.ident)) > -1)
			newConnection(eventSocket, _evList.ident);
		else if (_evList.filter == EVFILT_READ)
		{
			std::cout << "Data size to be read: " << _evList.data << std::endl;
			readEvent(servers);
		}
		else if (_evList.filter == EVFILT_WRITE)
			writeEvent();
		//CATCH
		// KEVENT -> INTERNAL_SERVER_ERROR, NOT AN EXCEPTION
	}
	_running = false;
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
			Socket sock(servers.at(i).getHost(j), servers.at(i).getPort(j), _kq, evSet);
			_sckts.push_back(sock);
		}
	}
	runWebserver(servers);
}

Webserver::~Webserver()
{
	std::map<int, Connection>::iterator it;
	for (it = _connections.begin(); it != _connections.end(); it++)
		close(it->second.getListenFd()); //protect!
}
