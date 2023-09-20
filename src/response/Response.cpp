#include "Response.hpp"
#include "Webserver.hpp"
#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include <cstdio>
#include <stdio.h>
#include <sys/stat.h>

std::map<int, std::string> 	Response::_responseCodes = 
{
	{OK, "OK"},
	{DELETED, "Deleted"},
	{MULTIPLE_CHOICES, "Multiple Choices"},
	{MOVED_PERMANENTLY, "Moved Permanently"},
	{TEMPORARY_REDIRECT, "Temporary Redirect"},
	{PERMANENT_REDIRECT, "Permanent Redirect"},
	{BAD_REQUEST, "Bad Request"},
	{FORBIDDEN, "Forbidden"},
	{NOT_FOUND, "Not Found"},
	{METHOD_NOT_ALLOWED, "Method Not Allowed"},
	{REQUEST_TIMEOUT, "Request Timeout"},
	{CONTENT_TOO_LARGE, "Content Too Large"},
	{INTERNAL_SERVER_ERROR, "Internal Server Error"},
	{NOT_IMPLEMENTED, "Not Implemented"}
};

/* TO BE DELETED */
void	Response::printResponse(void) const
{
	std::cout << "***RESPONSE***";
	std::cout << "\n\tStatus code: " << this->_statusCode;
	std::cout << "\n\tReason: " << this->_responseCodes[this->_statusCode];
	std::cout << "\n\tProtocol Version: " << this->_req.getProtocolVersion();
	std::cout << "\n\tConnection FD: " << this->_req.getConnFD();
	std::cout << "\n******" << std::endl;
}

void	Response::sendResponse(void)
{
	ssize_t bytesSent;
	ssize_t chunkSize = std::min(this->_fullResponse.length(), \
		static_cast<size_t>(MAXLINE));
	if (this->_state == SENDING)
	{
		bytesSent = send(this->_req.getConnFD(), this->_fullResponse.c_str(), \
			chunkSize, 0);
		if (bytesSent < 0)
		{
			_state = DONE;
			close(this->_req.getConnFD());
			return ;
		}
		else
			this->_fullResponse.erase(0, bytesSent);
		if (bytesSent == 0)
		{
			this->setState(DONE);
		}
	}
}

void	Response::prepareContent(std::ifstream	&file)
{
	std::string		body;
	
	if (this->_filePath.empty())
	{
		this->_fullResponse.append(this->_message);
		return;
	}
	else
	{
		body = std::string(std::istreambuf_iterator<char>(file), \
			std::istreambuf_iterator<char>());
		this->_fullResponse.append(body);
	}
}

void	Response::prepareHeaderLocation(void)
{
	std::string		location;
	
	if (this->_location->getReturnLink().size() > 0)
		location = this->_location->getReturnLink();
	else if (this->_req.getTarget().find("styles.css") < std::string::npos)
		location = "/styles.css";
	else
		location = \
		this->_req.getTarget().substr(this->_req.getTarget().rfind("/images"));
	if (!location.empty())
		this->addToFullResponse("Location: " + location + "\r\n");
}

std::string	Response::prepareHeaderContentType(std::string const & root)
{
	std::string	extension = \
	this->_filePath.substr(this->_filePath.find_last_of('.') + 1);
	
	if (root == "data")
		return ("image/" + extension);
	else
		return ("text/" + extension);
}

size_t	Response::prepareHeaderContentLength(void)
{
	if (this->_fileLength == 0 && this->_message.length() > 0)
		return (this->_message.length());
	else
		return (this->_fileLength);
}

void	Response::prepareHeaders(std::string const & root)
{
	char		responseBuffer[RESPONSELINE + 1];
	std::string	contentType = "text/html";
	size_t		contentLength = this->prepareHeaderContentLength();

	std::memset(responseBuffer, 0, RESPONSELINE);

	if (!this->_filePath.empty())
		contentType = this->prepareHeaderContentType(root);
	snprintf(responseBuffer, RESPONSELINE, \
		"Content-Type: %s\r\nContent-Length: %zu\r\n", \
		contentType.c_str(), contentLength);
	this->addToFullResponse(&responseBuffer[0], std::strlen(responseBuffer));

	if (this->_location->getReturnLink().size() > 0 || \
	(this->_statusCode >= MULTIPLE_CHOICES && \
	this->_statusCode <= PERMANENT_REDIRECT))
		this->prepareHeaderLocation();
	this->addToFullResponse("\r\n");
}

void	Response::prepareFirstLine(void)
{
	char		responseBuffer[RESPONSELINE + 1];
	std::string	responseMessage;
	std::string	version = this->_req.getProtocolVersion();
	
	std::memset(responseBuffer, 0, RESPONSELINE + 1);
	if (this->_location->getReturnMessage().size() > 0)
		responseMessage = this->_location->getReturnMessage();
	else
	{
		try 
		{
			responseMessage = this->_responseCodes.at(this->_statusCode);
		}
		catch (std::exception &e)
		{
			responseMessage = "";
		}
	}
	snprintf(responseBuffer, RESPONSELINE, "%s %d %s\r\n", version.c_str(), \
		this->_statusCode, responseMessage.c_str());
	this->addToFullResponse(&responseBuffer[0], std::strlen(responseBuffer));
}

std::string	Response::getErrorPageUri(void)
{
	std::map<int, std::string> const & errorPages = \
	this->_location->getErrorPages();
	try
	{
		return (errorPages.at(this->_statusCode));
	}
	catch(const std::out_of_range& oor)
	{
		return (DEFAULT_ERROR_PAGE);
	}
}

void	Response::identifyErrorPage(Server const & server)
{
	if (isRequestedByCurl(this->_req.getHeaders()["User-Agent"]))
	{
		this->_filePath.clear();
		return;
	}
	this->_isReady = false;
	std::string	targetUri = this->getErrorPageUri();
	while (!this->_isReady)
	{
		this->_location = this->findLocationMatch(targetUri, server.getLocations());
		this->_filePath = this->_location->getRoot() + targetUri;
		if (!hasReadPermission(this->_filePath) && \
			targetUri == DEFAULT_ERROR_PAGE)
		{
			this->_filePath.clear();
			this->_message = \
				this->_responseCodes.at(this->_statusCode).c_str();
			this->_isReady = true;
		}
		else if (!hasReadPermission(this->_filePath))
			targetUri = DEFAULT_ERROR_PAGE;
		else
			this->_isReady = true;
	}
}

void	Response::checkIfRedirectNecessary()
{
	if ((this->_req.getTarget().find("styles.css") < std::string::npos && \
	this->_req.getTarget().find("/") != this->_req.getTarget().rfind("/")) || \
	(this->_req.getTarget().rfind("/images") < std::string::npos && \
	this->_req.getTarget().rfind("/images") > 0))
	{
		this->setStatusCode(TEMPORARY_REDIRECT);
		this->_filePath.clear();
	}
}

void	Response::checkIfReturn(void)
{
	if (this->_statusCode < INTERNAL_SERVER_ERROR && \
	this->_location->getReturnCode())
	{
		this->setStatusCode(this->_location->getReturnCode());
		this->setMessage(this->_location->getReturnMessage());
		this->setFilePath("");
	}
}

void	Response::prepareResponse(Server const & server)
{
	std::ifstream	file;

	this->checkIfReturn();
	if (this->_statusCode > BAD_REQUEST && this->_statusCode <= METHOD_NOT_ALLOWED)
		this->checkIfRedirectNecessary();
	if (this->_statusCode >= BAD_REQUEST)
			this->identifyErrorPage(server);
	if (this->_fullResponse.empty())
	{
		if (!this->_filePath.empty())
		{
			this->_fileLength = getFileSize(this->_filePath);
			file.open(this->_filePath, std::ifstream::in | std::ifstream::binary);
			if (!file.is_open())
				this->setStatusCode(INTERNAL_SERVER_ERROR);
		}
		this->prepareFirstLine();
		this->prepareHeaders(this->_location->getRoot());
		if (isContentAllowed(this->_statusCode))
			this->prepareContent(file);
		if (!this->_filePath.empty())
			file.close();
	}
	this->setState(SENDING);
}

void	Response::performGET(void) 
{
	std::string scriptName = this->_location->getCgiScriptName();
	std::string queryString = this->_req.getQueryString();

	if ((!scriptName.empty() && !queryString.empty()) || \
		scriptName.find('*') < std::string::npos)
		this->executeCgiScript();
}

void	Response::performDELETE(void)	
{
	this->setStatusCode(deleteFile(this->_req, this->_location));
}

void	Response::executeCgiScript(void)
{
	if (this->getState() == PENDING)
	{
		CGI	cgi(this->_req);
		this->_cgi = cgi;
		
		std::string scriptName = this->_location->getCgiScriptName();
		if (scriptName.find('*') < std::string::npos)
			scriptName = this->_filePath;
		_cgi.prepareEnv(scriptName, *this);
		_cgi.prepareArg(scriptName);
		_cgi.run(*this);
	}
	if (getState() == READ_CGI)
		_cgi.cgiRead(*this, this->_fullResponse);
	else if (getState() == WRITE_CGI)
		_cgi.cgiWrite(*this);
}

void	Response::performPOST(void)
{
	size_t				bodyLength = this->_req.getBodyLength();
	unsigned long long	maxBodySize = this->_location->getMaxBodySize();

	if (this->getState() == PENDING)
	{
		if (bodyLength > maxBodySize && maxBodySize > 0)
		{
			this->setStatusCode(CONTENT_TOO_LARGE);
			return ;
		}
	}
	this->executeCgiScript();
}

void	Response::checkIfMethodAllowed()
{
	for (size_t i = 0; i < this->_location->getAllowed().size(); i++)
	{
		if (this->_location->getAllowed().at(i) == this->_req.getMethod())
			return ;
	}
	for (size_t i = 0; i < this->_location->getDenied().size(); i++)
	{
		if (this->_location->getDenied().at(i) == this->_req.getMethod() || \
		this->_location->getDenied().at(i) == "all")
		{
			this->setStatusCode(METHOD_NOT_ALLOWED);
			std::cout << "Location: " << this->_location->getMatch() << std::endl;		
			return ;
		}
	}
	return ;
}

void Response::checkIfGetIsActuallyDelete(void)
{
	if (this->_req.getMethod() == "GET" && \
	this->_req.getTarget() == "/deleted.html" && \
	this->_req.getQueryString() != "")
	{
		this->_req.setMethod("DELETE");
	}
}

void	Response::performRequest(void)
{
	this->checkIfGetIsActuallyDelete();
	this->checkIfMethodAllowed();
	// std::cerr << "Method is: " << this->_req.getMethod() << std::endl;
	if (this->_statusCode == OK)
	{
		if (this->_req.getMethod() == "POST")
			this->performPOST();
		else if (this->_req.getMethod() == "DELETE")
			this->performDELETE();
		else if (this->_req.getMethod() == "GET")
			this->performGET();
		else
			this->setStatusCode(NOT_IMPLEMENTED);
	}	
}

void	Response::extractPathInfo(std::string & targetUri)
{
	size_t		beginPathInfo = 0;
	std::string	needle;

	if (this->_location->getMatch()[0] == '*')
	{
		needle = this->_location->getMatch().substr(1);
		beginPathInfo = targetUri.find(needle) + needle.length();
		this->_pathInfo = targetUri.substr(beginPathInfo);
		targetUri.erase(beginPathInfo);
	}
}

std::string	Response::findIndexPage(locIterator itLoc)
{
	std::string					filePath;
	std::vector<std::string>	indexes = itLoc->getIndexes();

	for (auto itIdx = indexes.begin(); itIdx != indexes.end(); itIdx++)
	{
		filePath = itLoc->getRoot() + *itIdx;
		if (access(filePath.c_str(), F_OK) == 0)
			return (*itIdx);
	}
	this->setStatusCode(NOT_FOUND);
	throw std::ios_base::failure("Index file not found");
}

void	Response::prepareFilePath(std::string & targetUri)
{
	std::cerr << "TargetUri: " << targetUri << std::endl;
	if (targetUri[targetUri.length() - 1] == '/')
	{
		if (this->_req.getMethod() == "GET" && \
		!this->_location->getIndexes().empty())
			targetUri = this->findIndexPage(this->_location);
		else 
		{
			if (this->_location->getAutoindex())
				this->_message = createAutoindex(*this);
			else
				this->setStatusCode(NOT_FOUND);
			this->_isReady = true;
		}
	}
	else
	{
		this->extractPathInfo(targetUri);
		this->_filePath = this->_location->getRoot() + targetUri;
		if (!hasReadPermission(this->_filePath))
			this->setStatusCode(NOT_FOUND);
		this->_isReady = true;
	}
}

locIterator Response::findLocationMatch(std::string target, \
	std::vector<Location> const & locations)
{
	locIterator	itLoc;

	itLoc = findExactLocationMatch(target, locations);
	if (itLoc == locations.end())
		itLoc = findWildcardLocationMatch(target, locations);
	if (itLoc == locations.end())
		itLoc = findClosestLocationMatch(target, locations);
	if (itLoc == locations.end())
	{
		this->setStatusCode(INTERNAL_SERVER_ERROR);
		throw(std::range_error("No location match"));
	}
	return (itLoc);
}

void	Response::processTarget(Server const & server)
{
	std::string	targetUri = this->_req.getTarget();
	while (!this->_isReady)
	{
		try 
		{
			this->_location = this->findLocationMatch(targetUri, \
				server.getLocations());
			this->prepareFilePath(targetUri);
		}
		catch(const std::ios_base::failure & f)
		{
			this->setStatusCode(NOT_FOUND);
		}
		catch(const std::range_error &re)
		{
			this->setStatusCode(INTERNAL_SERVER_ERROR);
		}
	}
}
