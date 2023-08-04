#include "Connection.hpp"
#include "Socket.hpp"

Connection::Connection()
{

}

Connection::~Connection()
{

}

Connection::Connection(int listenfd, Socket sckt) : _listenfd(listenfd)
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handler = nullptr;
	this->_address = sckt.getAddress();
	this->_port = sckt.getPort();
}

Connection::Connection(const Connection &src)
{
	*this = src;
}

Connection& Connection::operator=(const Connection &src)
{
	this->_newReq = src._newReq;
	this->_newResp = src._newResp;
	this->_handler = src._handler;
	this->_listenfd = src._listenfd;
	this->_address = src._address;
	this->_port = src._port;
	// std::cout << "Copy assignment operator called on Server" << std::endl;
	return (*this);
}

void	Connection::handleRequest(int connfd, std::vector<Server> servers)//, Server *& handler, Request *& newReq)
{
	// Request		*newReq;
	// Response	*newResp; //moved to class Webserver

	try
	{
		_newReq = new Request(connfd);
		std::cout << "After Request constructor" << std::endl;
		_newReq->processReq();
		_newReq->printRequest();
		std::cout << " Address/host in Connection is: [" << _address << "]" << std::endl;
		_handler = new Server(_newReq->identifyServer(servers)); //BS: hier kun je het IP e.d. meenemen uit de Connection class
		std::cout << "_Handler info: host: [" << _handler->getPort(0) << "], port: [" << _handler->getHost(0) << "]" << std::endl;
		std::cout << "Responsible server is " << \
		_handler->getServerName(0) << std::endl;
		// newResp = new Response(*_newReq);
		// if (newReq->getMethod() == "POST")
		// {
		// 	//std::cout << "\n\nFULLREQUEST: [" << newReq->getFullRequest() << "]" << std::endl;
		// 	//Hier werkt request value nog
		// 	newResp->prepareResponsePOST(handler);
		// 	delete newReq;
		// }
		// else
		// {
		// 	delete newReq;
		// 	newResp->prepareResponseGET(handler);
		// }
		// delete newResp;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! " << e.what() << '\n';
	}
}


void	Connection::handleResponse()//Request *& newReq, Response *& newResp, Server *& handler)//int connfd, std::vector<Server> servers)
{
	// Request		*newReq;
	// Response	*newResp; //moved to class Webserver
			std::cout << "Responsible SERVER size in handleResponse is " << \
			_handler->getServerNames().size() << std::endl;

	try
	{
		_newResp = new Response(*_newReq);
		if (_newReq->getMethod() == "POST")
		{
			//std::cout << "\n\nFULLREQUEST: [" << _newReq->getFullRequest() << "]" << std::endl;
			//Hier werkt request value nog
			_newResp->prepareResponsePOST(*_handler);
			delete _newReq;
		}
		else
		{
			std::cout << "Responsible SERVER size in _handleResponse is " << \
			_handler->getServerNames().size() << std::endl;

			_newResp->prepareResponseGET(*_handler);
			delete _newReq;
		}
		delete _newResp;
		delete _handler;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! " << e.what() << '\n';
	}
}
