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

Connection::Connection(int listenfd, Socket sckt) : _listenfd(listenfd)
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
		std::cout << "Responsible server is " << \
		this->_handlingServer->getServerName(0) << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "!!! PROBABLY STH NEEDS TO BE ADDED TO CONFIG FILE - " << e.what() << '\n';
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

void	Connection::handleResponse()
{
<<<<<<< HEAD
	std::string method;
=======
	if (this->_newReq->getMethod() == "")
		return;

	// if unknown response type, return BAD REQUEST
>>>>>>> 06dffc7313bd2c98766f8c8e7e31fa2e2b3bb2e2

	try
	{
		this->_newResp = new Response(*this->_newReq);
		this->_newResp->prepareTargetURI(*this->_handlingServer);

		method = this->_newReq->getMethod();
		if (method == "GET" && this->_newReq->getTarget() == "/deleted.html" && this->_newReq->getQueryString() != "")
			method = "DELETE";
		if (!allowedInLocation(method, this->_newResp->getLocation())) //what about GET /deleted.html?
		{
			std::cout << "Method not allowed! " << this->_newReq->getMethod() << " in " << this->_newResp->getLocation()->getMatch() << std::endl;
			
			this->_newResp->setFilePath("data/www/defaultError.html"); // JMA: only when request is from browser, otherwise ""
			this->_newResp->setStatusCode(NOT_ALLOWED);
			//this should be something that overwrites all variables that matter for the response sending
		}
<<<<<<< HEAD
		else
=======
		//insert tests of allowed methods

		std::cerr << "method is " << this->_newReq->getMethod() << std::endl;
		if (this->_newReq->getMethod() == "POST")
		{
			this->_newResp->prepareResponsePOST();
		}
		else if (this->_newReq->getMethod() == "GET")
		{	
			this->_newResp->prepareResponseGET();
		}
		else if (this->_newReq->getMethod() == "DELETE" || \
			(this->_newReq->getMethod() == "GET" && \
			this->_newReq->getTarget() == "/deleted.html" && \
			this->_newReq->getQueryString() != ""))
>>>>>>> 06dffc7313bd2c98766f8c8e7e31fa2e2b3bb2e2
		{
			if (this->_newReq->getMethod() == "POST")
			{
				this->_newResp->prepareResponsePOST();
			}
			else if (method == "DELETE")
			{
				this->_newResp->prepareResponseDELETE();
			}	
			else if (this->_newReq->getMethod() != "GET")
			{
				std::cout << "I can't handle the \"" << this->_newReq->getMethod() << "\" method, sorry!" << std::endl;
				return;
			}
		}

		//RETURN CHECK
		// if (this->_newReq->getMethod() == "GET" || this->_newReq->getMethod() == "DELETE")
		// {			
			if (this->_newResp->getLocation()->getReturn().first != 0)
			{
				std::cout << "RETURN!!!!!!!!!!" << std::endl;
				this->_newResp->setStatusCode(this->_newResp->getLocation()->getReturn().first);
				//this->_newResp->setMessage(this->_newResp->getLocation()->getReturn().second)// JMA: where do I store the return message? This still needs to be done
				this->_newResp->setFilePath("");
			}

		// }
		this->_newResp->sendResponse();
		this->_newReq->setState(OVERWRITE);
		delete this->_newResp;
		delete this->_handlingServer;
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
