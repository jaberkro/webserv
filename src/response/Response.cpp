#include "Response.hpp"
#include "Webserver.hpp"
#include "CGI.hpp"
#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include <cstdio>
#include <stdio.h>
#include <sys/stat.h>

/**
 * @brief static variable _responseCodes is a map containing all reasons that 
 * belong to various status codes. These are used in the response sent to the 
 * client.
 */
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
	{CONTENT_TOO_LARGE, "Content Too Large"},
	{INTERNAL_SERVER_ERROR, "Internal Server Error"},
	{NOT_IMPLEMENTED, "Not Implemented"}
};

void	Response::processTarget(Server const & server)
{
	// std::string	targetUri = this->_req.getTarget().substr(0, \
	// 	this->_req.getTarget().find_first_of('?')); // JMA: can this go?
	std::string	targetUri = this->_req.getTarget();
	while (!this->_isReady)
	{
		try 
		{
			this->_location = findLocationMatch(targetUri, \
				server.getLocations());
			this->prepareFilePath(targetUri);
		}
		catch(const std::ios_base::failure & f)
		{
			std::cerr << "IOS exception caught (something went wrong with ";
			std::cerr << "opening the file " << this->_filePath << "): "; // DEBUG - TO BE DELETED
			std::cerr << f.what() << std::endl;
			this->setStatusCode(NOT_FOUND);
		}
		catch(const std::range_error &re)
		{
			std::cerr << "Range exception caught: (no location match found ";
			std::cerr << "for target " << this->_req.getTarget() << "): "; // DEBUG - TO BE DELETED
			std::cerr << re.what() << std::endl;
			this->setStatusCode(INTERNAL_SERVER_ERROR);
		}
	}
}

void	Response::prepareFilePath(std::string & targetUri)
{
	std::cerr << "TargetUri: " << targetUri << std::endl; // DEBUG - TO BE DELETED // JMA: maybe keep it
	if (targetUri[targetUri.length() - 1] == '/')
	{
		if (this->_req.getMethod() == "GET" && \
		!this->_location->getIndexes().empty())
			targetUri = findIndexPage(this->_location);
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

void	Response::performRequest(void)
{
	std::cerr << "Method: " << this->_req.getMethod() << std::endl;
	if (this->_req.getMethod() == "POST")
		this->performPOST();
	else if (this->_req.getMethod() == "DELETE")
		this->performDELETE();
	else if (this->_req.getMethod() == "GET")
		this->performGET();
	else
		this->setStatusCode(NOT_IMPLEMENTED);
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
	// if (this->_statusCode == DELETED) // JMA: needed? Guess not
	// 	this->_filePath.clear();
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
	if (this->getState() == READ_CGI)// && _cgi.checkIfCgiPipe())
		_cgi.cgiRead(*this, this->_fullResponse);
	else if (this->getState() == WRITE_CGI)// && _cgi.checkIfCgiPipe())
		_cgi.cgiWrite(*this);
}

void	Response::prepareResponse(Server const & server)
{
	std::ifstream	file;

	if (this->_statusCode > BAD_REQUEST && this->_statusCode <= METHOD_NOT_ALLOWED)
		this->checkIfRedirectNecessary();
	if (this->_statusCode >= 400)
			this->identifyErrorPage(server);
	if (this->_fullResponse.empty()) // here we check whether response was already prepared by a CGI script
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
		if (this->_statusCode > 199 && this->_statusCode != DELETED && \
		this->_statusCode != 304) // JMA & DM: put in bool allowedToHaveContent() function to make it more readable
			this->prepareContent(file);
		if (!this->_filePath.empty())
			file.close();
	}
	this->setState(SENDING);
}

void	Response::sendResponse(void)
{
	ssize_t bytesSent;
	ssize_t chunkSize = std::min(this->_fullResponse.length(), \
		static_cast<size_t>(MAXLINE)); // JMA: can min fail?
	// std::cout << "Chunksize is " << _fullResponse.length() << " or " << static_cast<size_t>(MAXLINE) << std::endl;
	if (this->_state == SENDING)
	{
		// std::cerr << "[sendResponse] SENDING to fd " << this->_req.getConnFD();// DEBUG - TO BE DELETED
		// std::cerr << ", status code is " << this->_statusCode << std::endl;// DEBUG - TO BE DELETED
		bytesSent = send(this->_req.getConnFD(), this->_fullResponse.c_str(), \
			chunkSize, 0);
		if (bytesSent < 0)
		{
			std::cout << "BytesSent error, send internal error" << std::endl;// DEBUG - TO BE DELETED
			// this->_state = ERROR; // JMA: should it be like this?
			// CLOSE CONNECTION
			return ;
		}
		this->_fullResponse.erase(0, bytesSent);
		// std::cout << "State is " << this->_state << ", bytesSent = ";// DEBUG - TO BE DELETED
		// std::cout << bytesSent << ", response leftover size is ";// DEBUG - TO BE DELETED
		// std::cout << this->_fullResponse.size() << ", chunkSize = ";// DEBUG - TO BE DELETED
		// std::cout << chunkSize << std::endl;// DEBUG - TO BE DELETED
		if (bytesSent == 0)
		{
			this->setState(DONE);
		}
	}
	// BOUNCE CLIENT WHEN: INTERNAL_SERVER_ERROR
}

void	Response::checkIfRedirectNecessary()
{
	std::cerr << "[preparing response] match is " << this->_location->getMatch() << ", status code is " << this->_statusCode << std::endl; // DEBUG - TO BE DELETED

	if ((this->_req.getTarget().find("styles.css") < std::string::npos && \
	this->_req.getTarget().find("/") != this->_req.getTarget().rfind("/")) || \
	(this->_req.getTarget().rfind("/images") < std::string::npos && \
	this->_req.getTarget().rfind("/images") > 0))
	{
		this->setStatusCode(TEMPORARY_REDIRECT);
		this->_filePath.clear();
	}
}

void	Response::identifyErrorPage(Server const & server)
{
	std::string	targetUri = getErrorPageUri();

	std::cerr << "[identifyErrorPage] " << std::endl; // DEBUG - TO BE DELETED
	this->_isReady = false;
	while (!this->_isReady)
	{
		this->_location = findLocationMatch(targetUri, server.getLocations());
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

/**
 * @brief finds a location match for the target URI
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance 
 * that is either the exact match for the target, if available, or the 
 * closest one.
 */
locIterator Response::findLocationMatch(std::string target, \
	std::vector<Location> const & locations)
{
	locIterator	itLoc;

	// std::cerr << "Finding a location for target " << target << ", len locations is " << locations.size() << std::endl; // DEBUG - TO BE DELETED
	itLoc = findExactLocationMatch(target, locations);
	if (itLoc == locations.end())
		itLoc = findWildcardLocationMatch(target, locations);
	if (itLoc == locations.end())
		itLoc = findClosestLocationMatch(target, locations);
	if (itLoc == locations.end())
	{
		this->setStatusCode(INTERNAL_SERVER_ERROR);
		throw(std::range_error("No location match")); // further handle
	}
	// std::cerr << "Found location " << itLoc->getMatch() << std::endl; // DEBUG - TO BE DELETED
	return (itLoc);
}

/**
 * @brief loops through the index page filenames provided in the config file 
 * for a particular location. For each one, it constructs the path to the file 
 * and tries to locate the file. If successful, it returns the filename of 
 * that index page. If none of the files is found, the status code is set to 
 * 404 (Not Found) and an exception is thrown.
 * 
 * @param itLoc iterator to the relevant Location object
 * @return std::string - the filename of the index corresponding to the 
 * location. 
 */
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
		// std::cerr << "[pathInfo extraction] targetUri is " << targetUri; // DEBUG - TO BE DELETED
		// std::cerr << ", pathInfo is " << this->_pathInfo << std::endl; // DEBUG - TO BE DELETED
	}
}

/**
 * @brief composes the first line of a response, writing it into the response 
 * buffer. After sending the response, it clears the buffer with std::memset().
 * 
 * @param response the buffer into which the response is written
 */
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

/**
 * @brief composes the headers of a response, writing them into the response buffer.
 * After sending, it clears the buffer with std::memset().
 * 
 * @param response the buffer into which the response is written
 * @param root the root directory (from the location block; used to distinguish
 * between the file types that are to be returned - to determine Content Type)
 */
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
