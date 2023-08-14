#include "Connection.hpp"
#include "Socket.hpp"

Connection::Connection()
{
	// std::cout << "Default constructor called on Connection" << std::endl;
}

Connection::~Connection()
{
	// std::cout << "Destructor called on Connection" << std::endl;
}

Connection::Connection(int listenfd, Socket sckt) : _listenfd(listenfd)
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handler = nullptr;
	this->_address = sckt.getAddress();
	this->_port = sckt.getPort();
	// std::cout << "Parametric constructor called on Connection" << std::endl;
}

Connection::Connection(const Connection &src)
{
	*this = src;
	// std::cout << "Copy constructor called on Connection" << std::endl;
}

Connection& Connection::operator=(const Connection &src)
{
	this->_newReq = src._newReq;
	this->_newResp = src._newResp;
	this->_handler = src._handler;
	this->_listenfd = src._listenfd;
	this->_address = src._address;
	this->_port = src._port;
	// std::cout << "Copy assignment operator called on Connection" << std::endl;
	return (*this);
}

void	Connection::handleRequest(int connfd, std::vector<Server> servers)
{
	try
	{

		if (!this->_newReq)
			_newReq = new Request(connfd, this->_address);
		std::cout << "After Request constructor" << std::endl;
		this->_newReq->processReq();
		this->_newReq->printRequest();
		std::cout << " Address/host in Connection is: [" << this->_address << "]" << std::endl;
		this->_handler = new Server(this->_newReq->identifyServer(servers)); //BS: hier kun je het IP e.d. meenemen uit de Connection class
		std::cout << "_Handler info: host: [" << this->_handler->getPort(0) << "], port: [" << this->_handler->getHost(0) << "]" << std::endl;
		std::cout << "Responsible server is " << \
		this->_handler->getServerName(0) << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! " << e.what() << '\n';
	}
}

void	Connection::handleResponse()
{
	std::cout << "Responsible SERVER size in handleResponse is " << \
	this->_handler->getServerNames().size() << std::endl;

	try
	{
		this->_newResp = new Response(*_newReq);
		this->_newResp->prepareTargetURI(*_handler);

		//insert tests of allowed methods

		if (this->_newReq->getMethod() == "POST")
		{
			this->_newResp->prepareResponsePOST(*_handler);
		}
		else if (this->_newReq->getMethod() == "DELETE" || (_newReq->getMethod() == "GET" && this->_newReq->getTarget() == "/deleted.html" && this->_newReq->getQueryString() != ""))
		{
			this->_newResp->prepareResponseDELETE(*_handler);
		}
		else
		{
			this->_newResp->prepareResponseGET(*_handler);
		}
		delete _newReq;
		delete _newResp;
		delete _handler;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! " << e.what() << '\n';
	}
}
