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
		std::cout << "Responsible server is "; // DEBUG - TO BE DELETED
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
		return;
	try
	{
		if (this->_newResp == nullptr) // DM: shouldn't we replace this by a state?
		{
			this->_newResp = new Response(*this->_newReq);
			if (this->_newReq->getState() == REQ_ERROR)
				this->_newResp->setError(this->_newReq->getStatusCode());
	// DM starting from here this should be only if state == PENDING (also, this needs to be split into separate functions)
			this->_newResp->processTarget(*this->_handlingServer);
			if (getIsActuallyDelete(this->_newReq))
				this->_newReq->setMethod("DELETE");
			if (!allowedInLocation(this->_newReq->getMethod(), this->_newResp->getLocation()))
			{
				this->_newResp->setStatusCode(METHOD_NOT_ALLOWED);
				std::cout << "Method not allowed! " << this->_newReq->getMethod() << " in " << this->_newResp->getLocation()->getMatch() << std::endl; // JMA: remove later? // DEBUG - TO BE DELETED
				// DM the below can be removed
				if (this->_newResp->getRequest().getHeaders()["User-Agent"].find("curl") == 0)
					this->_newResp->setFilePath("");
			}

			else if (this->_newResp->getStatusCode() == OK)
				this->_newResp->performRequest();
			// DM: does the below belong in the scope of the else{} statement?
			//JMA: maybe returnCode, returnLink and returnMessage? seperate, because the message can also be empty
			if (this->_newResp->getLocation()->getReturnCode()) // DM should we only do this for status codes < 500? // JMA: good question. Maybe also above 500 should be allowed, up to the creator of the config file to make a valid file? (we can also forbid it in the parsing)
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
			std::cout << "changed state to OVERWRITE" << std::endl; // DEBUG - TO BE DELETED
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