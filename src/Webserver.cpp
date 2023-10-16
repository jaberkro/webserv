#include "Webserver.hpp"
#include <fcntl.h>
#include <csignal>

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
	{
		if (it->first == evFd)
			return (it->second);
	}
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
}

void	Webserver::addWriteFilter(int fd)
{
	EV_SET(&_evList, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
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
		throw Webserver::KeventError();
}

void	Webserver::eofEvent(int ident)
{
	if (fcntl(ident, F_GETFD) > -1)
	{
		std::cout << "Disconnect for fd " << ident;
		_connections[ident].setTimer(false);
		std::cout << std::endl;
		if (close(ident) < 0)
			throw Webserver::CloseError();
	}
}

void	Webserver::newConnection(int eventSocket, int ident)
{
	int connfd;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);

	if ((connfd = accept(ident, (struct sockaddr *)&addr, &socklen)) < 0)
		throw Webserver::AcceptError();
	this->_connections[connfd] = Connection(ident, _sckts.at(eventSocket));
	this->_connections[connfd].setRequest(new Request(connfd, _sckts.at(eventSocket).getAddress()));
	if (fcntl(connfd, F_SETFL, O_NONBLOCK) < 0)
		this->_connections[connfd].getRequest()->setError(INTERNAL_SERVER_ERROR);
	addReadFilter(connfd);
}

void	Webserver::readEvent(std::vector<Server> servers)
{
	int evFd = checkIfCgiFd((int)_evList.ident);
	if (this->_connections[evFd].getResponse())
	{
		if (this->_connections[evFd].getResponse()->getState() == READ_CGI)
			this->_connections[evFd].handleResponse();
	}
	else
	{
		this->_connections[(int)evFd].handleRequest(evFd, servers, (int)_evList.data);
		if (this->_connections[(int)evFd].getRequest() && (this->_connections[(int)evFd].getRequest()->getState() == WRITE || this->_connections[(int)evFd].getRequest()->getState() == REQ_ERROR))
			addWriteFilter(evFd);
	}
}

void	Webserver::writeEvent()
{
	int evFd = checkIfCgiFd((int)_evList.ident);
	_connections[evFd].handleResponse();
	if (_connections[evFd].getResponse() == nullptr)//In case the Response is sent and finished, the write filter can be deleted
		deleteWriteFilter(evFd);
	else if (_connections[evFd].getResponse()->getState() == INIT_CGI && _connections[evFd].getResponse()->cgiOnKqueue == false)
	{
		addWriteFilter(_connections[evFd].getResponse()->getCgi().getScriptToWebserv()[0]);
		addWriteFilter(_connections[evFd].getResponse()->getCgi().getWebservToScript()[1]);
		_connections[evFd].getResponse()->cgiOnKqueue = true;
		_connections[evFd].getResponse()->setState(WRITE_CGI);
		_cgiFds.insert({evFd, _connections[evFd].getResponse()->getCgi().getScriptToWebserv()[0]});
		_cgiFds.insert({evFd, _connections[evFd].getResponse()->getCgi().getWebservToScript()[1]});
	}
	addTimerFilter(evFd);
}

void	Webserver::runWebserver(std::vector<Server> servers)
{
	int nev;
	int	eventSocket;

	while (1)
	{
		try 
		{
			struct timespec *timeout = NULL;
			_running = true;
			if ((nev = kevent(_kq, NULL, 0, &_evList, 1, timeout)) < 0)
				continue ;
			if (_evList.filter == EVFILT_TIMER || _evList.flags & EV_EOF)
				eofEvent(_evList.ident);
			else if ((eventSocket = comparefd((int)_evList.ident)) > -1)
				newConnection(eventSocket, _evList.ident);
			else if (_evList.filter == EVFILT_READ)
				readEvent(servers);
			else if (_evList.filter == EVFILT_WRITE)
				writeEvent();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << '\n';
		}
	}
	_running = false;
}

void	sighandler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << "\nWebserver will be stopped now, adieu!" << std::endl;
		exit(0);
	}
}

void	Webserver::setSignal()
{
	std::signal(SIGINT, sighandler);
}

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
			try 
			{
				std::cout << "Port: [" << servers.at(i).getPort(j) << "], host: [" << \
				servers.at(i).getHost(j) << "], servername(s):";
				for (size_t k = 0; k < servers.at(i).getServerNames().size(); k++)
					std::cout << " [" << servers.at(i).getServerName(k) << "]";	
				std::cout << std::endl;			
				Socket sock(servers.at(i).getHost(j), servers.at(i).getPort(j), _kq, evSet);
				_sckts.push_back(sock);
			}
			catch(const std::exception& e)
			{
				if (errno != EADDRINUSE)
					std::cerr << "Exception when initializing socket: " << e.what() << '\n';
			}
		}
	}
	runWebserver(servers);
}

Webserver::~Webserver()
{
	std::map<int, Connection>::iterator it;
	for (it = _connections.begin(); it != _connections.end(); it++)
	{
		if (close(it->second.getListenFd()) < 0)
			std::cout << "Closing fd in Webserver destructor failed" << std::endl;
	}
}
