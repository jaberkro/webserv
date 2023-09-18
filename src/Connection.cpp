#include "Webserver.hpp"
#include "Connection.hpp"
#include "Socket.hpp"

Connection::Connection()
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handlingServer = nullptr;
	this->_address = "";
	this->_port = 0;
	this->_timer = false;
	// std::cout << "Default constructor called on Connection" << std::endl;
}

Connection::~Connection()
{
	// std::cout << "Destructor called on Connection" << std::endl;
}

Connection::Connection(int listenfd, Socket sckt) : _listenfd(listenfd) // ADD STATUSCODE
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handlingServer = nullptr;
	this->_address = sckt.getAddress();
	this->_port = sckt.getPort();
	this->_timer = false;
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
	this->_handlingServer = src._handlingServer;
	this->_listenfd = src._listenfd;
	this->_address = src._address;
	this->_port = src._port;
	// std::cout << "Copy assignment operator called on Connection" << std::endl;
	return (*this);
}

void	Connection::handleRequest(int connfd, std::vector<Server> servers)
{
	if (this->_newReq->getState() == REQ_ERROR)
		return ;

	try
	{
		if (this->_newReq->getState() == OVERWRITE)
		{
			delete this->_newReq;
			this->_newReq = new Request(connfd, this->_address);
		}
		this->_newReq->processReq();
		this->_newReq->printRequest(); // DEBUG - TO BE DELETED
		this->_handlingServer = new Server(this->_newReq->identifyServer(servers));
		std::cout << "Server: "; // DEBUG - TO BE DELETED
		std::cout << this->_handlingServer->getServerName(0) << std::endl; // DEBUG - TO BE DELETED
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! [handleRequest] this exception should not be happening: "; // DEBUG - TO BE DELETED
		std::cerr << e.what() << '\n'; // DEBUG - TO BE DELETED
		this->_newReq->setState(REQ_ERROR);
		this->_newReq->setStatusCode(INTERNAL_SERVER_ERROR);
	}
}

void	Connection::checkIfMethodAllowed(std::string method, locIterator location)
{
	for (size_t i = 0; i < location->getAllowed().size(); i++)
	{
		if (location->getAllowed().at(i) == method)
			return ;
	}
	for (size_t i = 0; i < location->getDenied().size(); i++)
	{
		if (location->getDenied().at(i) == method || location->getDenied().at(i) == "all")
		{
			this->_newResp->setStatusCode(METHOD_NOT_ALLOWED);
			std::cout << "Location: " << location->getMatch() << std::endl;		
			return ;
		}
	}
	return ;
}

void Connection::checkIfGetIsActuallyDelete(Request &request)
{
	if (request.getMethod() == "GET" && \
	request.getTarget() == "/deleted.html" && \
	request.getQueryString() != "")
	{
		request.setMethod("DELETE");
	}
}

void	Connection::handleResponse()
{
	if (this->_newReq->getMethod() == "")
		return;
	try
	{
		if (this->_newResp == nullptr)
		{
			this->_newResp = new Response(*this->_newReq);
			if (this->_newReq->getState() == REQ_ERROR)
				this->_newResp->setError(this->_newReq->getStatusCode());
	// DM starting from here this should be split into separate functions)
			this->_newResp->processTarget(*this->_handlingServer);
			checkIfGetIsActuallyDelete(this->_newResp->getRequest());
			checkIfMethodAllowed(this->_newResp->getRequest().getMethod(), this->_newResp->getLocation());
			
			if (this->_newResp->getStatusCode() == OK)
				this->_newResp->performRequest();
			if (this->_newResp->getStatusCode() < INTERNAL_SERVER_ERROR && \
			this->_newResp->getLocation()->getReturnCode())
			{
				this->_newResp->setStatusCode(this->_newResp->getLocation()->getReturnCode());
				this->_newResp->setMessage(this->_newResp->getLocation()->getReturnMessage());
				this->_newResp->setFilePath("");
			}
		}
		if (this->_newResp->getState() == WRITE_CGI || this->_newResp->getState() == READ_CGI)
			this->_newResp->executeCgiScript();
		if (this->_newResp->getState() == PENDING)
			this->_newResp->prepareResponse(*this->_handlingServer);
		if (this->_newResp->getState() == SENDING)
			this->_newResp->sendResponse();
		if (this->_newResp->getState() == DONE)
		{
			this->_newReq->setState(OVERWRITE);
			delete this->_newResp;
			this->_newResp = nullptr;
			delete this->_handlingServer;
	// DM up until here
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! " << e.what() << '\n';
	}
}

void	Connection::setRequest(Request *request)
{
	this->_newReq = request;
}

Request *	Connection::getRequest(void)
{
	return (this->_newReq);
}

Response *	Connection::getResponse(void)
{
	return (this->_newResp);
}

int		Connection::getListenFd()
{
	return (this->_listenfd);
}

bool	Connection::getTimer()
{
	return (this->_timer);
}

void	Connection::setTimer(bool state)
{
	this->_timer = state;
}