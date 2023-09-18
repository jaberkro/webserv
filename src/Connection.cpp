#include "Webserver.hpp"
#include "Connection.hpp"

Connection::Connection()
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handlingServer = nullptr;
	this->_address = "";
	this->_port = 0;
	this->_timer = false;
}

Connection::~Connection() {}

Connection::Connection(int listenfd, Socket sckt) : _listenfd(listenfd) // ADD STATUSCODE
{
	this->_newReq = nullptr;
	this->_newResp = nullptr;
	this->_handlingServer = nullptr;
	this->_address = sckt.getAddress();
	this->_port = sckt.getPort();
	this->_timer = false;
}

Connection::Connection(const Connection &src)
{
	*this = src;
}

Connection& Connection::operator=(const Connection &src)
{
	this->_newReq = src._newReq;
	this->_newResp = src._newResp;
	this->_handlingServer = src._handlingServer;
	this->_listenfd = src._listenfd;
	this->_address = src._address;
	this->_port = src._port;
	return (*this);
}
void	Connection::cleanUp(void)
{
	this->_newReq->setState(OVERWRITE);
	delete this->_newResp;
	this->_newResp = nullptr;
	delete this->_handlingServer;
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
			this->_newResp->processTarget(*this->_handlingServer);
			this->_newResp->performRequest();
		}
		if (this->_newResp->getState() == WRITE_CGI || \
		this->_newResp->getState() == READ_CGI)
			this->_newResp->executeCgiScript();
		if (this->_newResp->getState() == PENDING || \
		this->_newResp->getState() == RES_ERROR)
			this->_newResp->prepareResponse(*this->_handlingServer);
		if (this->_newResp->getState() == SENDING)
			this->_newResp->sendResponse();
		if (this->_newResp->getState() == DONE)
			this->cleanUp();
	}
	/* DM: The aim is to remove the try-catch block from here 
	(all exceptions should be handled within all these sub-functions). 
	I'll do this later. */
	catch(const std::exception& e) 
	{
		std::cerr << "!!! " << e.what() << '\n';
	}
}

void	Connection::handleRequest(int connfd, std::vector<Server> servers, \
int dataSize)
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
		this->_newReq->processReq(dataSize);
		this->_newReq->printRequest(); // DEBUG - TO BE DELETED
		this->_handlingServer = new Server(this->_newReq->identifyServer(servers));
		std::cout << "Server: ";
		std::cout << this->_handlingServer->getServerName(0) << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl; // DEBUG - TO BE DELETED
		this->_newReq->setState(REQ_ERROR);
		this->_newReq->setStatusCode(INTERNAL_SERVER_ERROR);
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

int		Connection::getListenFd(void)
{
	return (this->_listenfd);
}

bool	Connection::getTimer(void)
{
	return (this->_timer);
}

void	Connection::setTimer(bool state)
{
	this->_timer = state;
}
