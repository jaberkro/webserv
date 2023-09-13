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
	try
	{
		if (this->_newReq->getState() == OVERWRITE)
		{
			delete this->_newReq;
			this->_newReq = new Request(connfd, this->_address);
		}
		// std::cout << "After Request constructor" << std::endl;
		this->_newReq->processReq();
		this->_newReq->printRequest();
		this->_handlingServer = new Server(this->_newReq->identifyServer(servers));
		std::cout << "_Handler info: host: [" << this->_handlingServer->getPort(0) << "], port: [" << this->_handlingServer->getHost(0) << "]" << std::endl;
		std::cout << "Responsible server is " << 
		this->_handlingServer->getServerName(0) << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! (MAYBE STH NEEDS TO BE ADDED TO CONFIG FILE?) - " << e.what() << '\n';
	}
}

static bool	allowedInLocation(std::string method, std::vector<Location>::const_iterator location)
{
	for (size_t i = 0; i < location->getAllowed().size(); i++)
	{
		if (location->getAllowed().at(i) == method)
			return (true);
	}
	for (size_t i = 0; i < location->getDenied().size(); i++)
	{
		if (location->getDenied().at(i) == method || location->getDenied().at(i) == "all")
			return (false);
	}
	return (true);
}

static bool getIsActuallyDelete(Request *request)
{
	if (request->getMethod() == "GET" && \
		request->getTarget() == "/deleted.html" && \
		request->getQueryString() != "")
	{
		return (1);
	}
	return (0);
}

void	Connection::handleResponse()
{
	if (this->_newReq->getMethod() == "")
		return; // JMA: we return here but that means we will also not send or delete the response. Is that a problem?

	try
	{
		if (this->_newResp == nullptr)
		{
			this->_newResp = new Response(*this->_newReq);
			this->_newResp->prepareTargetURI(*this->_handlingServer);

			if (getIsActuallyDelete(this->_newReq))
				this->_newReq->setMethod("DELETE");
			if (!allowedInLocation(this->_newReq->getMethod(), this->_newResp->getLocation()))
			{
				std::cout << "Method not allowed! " << this->_newReq->getMethod() << " in " << this->_newResp->getLocation()->getMatch() << std::endl; // JMA: remove later?
				if (this->_newResp->getRequest().getHeaders()["User-Agent"].find("curl") == 0)
					this->_newResp->setFilePath("");
				this->_newResp->setStatusCode(METHOD_NOT_ALLOWED);
				//this should be something that overwrites all variables that matter for the response sending
			}

			else if (this->_newResp->getStatusCode() == OK) // DM: only do this if the status code is OK (200)
			{
				std::cerr << "method is " << this->_newReq->getMethod() << std::endl;
				if (this->_newReq->getMethod() == "POST")
					this->_newResp->prepareResponsePOST();
				else if (this->_newReq->getMethod() == "DELETE")
					this->_newResp->prepareResponseDELETE();
				else if (this->_newReq->getMethod() == "GET")
					this->_newResp->prepareResponseGET();
				else
					this->_newResp->setStatusCode(NOT_IMPLEMENTED);
			}
			// DM: does the below belong in the scope of the else{} statement?
			//JMA: maybe returnCode, returnLink and returnMessage? seperate, because the message can also be empty
			if (this->_newResp->getLocation()->getReturnCode()) // DM should we only do this for status codes < 500?
			{
				this->_newResp->setStatusCode(this->_newResp->getLocation()->getReturnCode());
				this->_newResp->setMessage(this->_newResp->getLocation()->getReturnMessage());
				this->_newResp->setFilePath("");
			}
		}
		if (this->_newResp->getState() == WRITE_CGI || this->_newResp->getState() == READ_CGI)
			this->_newResp->prepareResponsePOST();
		if (this->_newResp->getStatusCode() >= 400)
			this->_newResp->identifyErrorPage(*this->_handlingServer); 
		if (this->_newResp->getState() == PENDING || this->_newResp->getState() == SENDING)
			this->_newResp->sendResponse();
		if (this->_newResp->getState() == DONE)
		{
			this->_newReq->setState(OVERWRITE);
			delete this->_newResp;
			this->_newResp = nullptr;
			delete this->_handlingServer;
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