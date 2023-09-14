#include "Response.hpp"

#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include "Webserver.hpp"
#include "CGI.hpp"
#include <cstdio>
#include <stdio.h>
#include <sys/stat.h>

#include <vector>

/**
 * @brief static variable _responseCodes is a map containing all reasons that 
 * belong to various status codes. These are used in the response sent to the 
 * client.
 */
std::map<int, std::string> 	Response::_responseCodes = 
{
	{OK, "OK"},
	{DELETED, "Deleted"},
	{MOVED_PERMANENTLY, "Moved Permanently"},
	{BAD_REQUEST, "Bad Request"},
	{FORBIDDEN, "Forbidden"},
	{NOT_FOUND, "Not Found"},
	{METHOD_NOT_ALLOWED, "Method Not Allowed"},
	{CONTENT_TOO_LARGE, "Content Too Large"},
	{INTERNAL_SERVER_ERROR, "Internal Server Error"},
	{NOT_IMPLEMENTED, "Not Implemented"}
};

void	Response::prepareResponseGET(void) 
{
	// std::cerr << "prepGET - cgi script name is " << this->_location->getCgiScriptName() << std::endl;
	if ((!this->_location->getCgiScriptName().empty() && !this->_req.getQueryString().empty()) || \
	this->_location->getCgiScriptName().find('*') < std::string::npos)
		this->executeCgiScript();
}

void	Response::prepareResponseDELETE(void)	
{
	uint8_t		response[RESPONSELINE + 1];

	std::memset(response, 0, RESPONSELINE);
	this->deleteFile();
	if (this->_statusCode != OK && this->_statusCode != DELETED)
	{
		if (this->_req.getHeaders()["User-Agent"].find("curl") == 0)
			this->_filePath.clear();
		else
			this->_filePath = "data/www/deleteFailed.html";
	}
}

/**
 * @brief execute the DELETE method on the target defined in the request.
 * Sets the _statusCode based on the result. OK or DELETED means that the
 * DELETE is executed succesfully.
 */
void	Response::deleteFile(void)
{
	std::string	toRemove;
	struct stat fileInfo;

	std::cout << "\nATTEMPT TO DELETE RIGHT NOW!!!" << std::endl;
	if (this->_req.getHeaders()["User-Agent"].find("curl") == 0)
		toRemove = this->_location->getRoot() + this->_req.getTarget();
	else
		toRemove = this->_location->getUploadDir() + "/" + this->_req.getQueryString().substr(this->_req.getQueryString().find_last_of("=") + 1);
	std::cout << "DELETE path: " << toRemove << "; TO REMOVE IS " << toRemove << std::endl;
	if (forbiddenToDeleteFileOrFolder(toRemove))
		this->_statusCode = FORBIDDEN;
	else if (stat(toRemove.c_str(), &fileInfo) != 0)	// DM: this seems not to be working
		this->_statusCode = NOT_FOUND;
	else if (fileInfo.st_mode & S_IFDIR || remove(toRemove.c_str()) != 0)
		this->_statusCode = BAD_REQUEST;
	else
	{
		std::cout << "DELETE SUCCESSFUL!\n" << this->_req.getHeaders()["User-Agent"] << "\n" << std::endl;
		if (this->_req.getHeaders()["User-Agent"].find("curl") == 0)
		{
			this->_statusCode = DELETED;
			this->_filePath.clear();
		}
		else
			this->_statusCode = OK;
	}
}

void	Response::prepareResponsePOST(void)
{
	std::cout << "Checking if POST is allowed based om the client_max_body_size..." << std::endl;
	std::cout << "max size: " << this->getLocation()->getMaxBodySize() << " and body length: " << this->getRequest().getBodyLength() << std::endl;
	if (this->getRequest().getBodyLength() > this->getLocation()->getMaxBodySize() && this->getLocation()->getMaxBodySize() > 0)
	{
		std::cout << "POST not allowed: Content Too Large. Max body size is " << this->getLocation()->getMaxBodySize() << std::endl;
		this->_statusCode = CONTENT_TOO_LARGE;
		if (this->_req.getHeaders()["User-Agent"].find("curl") == 0)
			this->_filePath.clear();
		else
			this->_filePath = "data/www/postFailed.html";
		return ;
	}
	this->executeCgiScript();

}

void	Response::executeCgiScript(void)
{
	if (this->_state == PENDING)
	{
		CGI	cgi(this->_req);
		this->_cgi = cgi;
		
		std::string scriptName = this->_location->getCgiScriptName();
		if (scriptName.find('*') < std::string::npos)
			scriptName = this->_filePath;
		_cgi.prepareEnv(scriptName, this->_pathInfo);
		_cgi.prepareArg(scriptName);
	}
	if (getState() == PENDING)
		_cgi.run(*this);
	if (getState() == READ_CGI)// && _cgi.checkIfCgiPipe())
		_cgi.cgiRead(*this, this->_fullResponse);
	else if (getState() == WRITE_CGI)// && _cgi.checkIfCgiPipe())
		_cgi.cgiWrite(*this);
}

/**
 * @brief Prepares the response to a GET method request. It contains a loop, in which 
 * (i) the relevant location is identified, 
 * (ii) if necessary, internal redirect is performed, 
 * (iii) the relevant location for the new target is identified, 
 * (iv) the path to the definitive target is constructed, and
 * (v) the function that retrieves the file is called.
 * 
 * @param server a reference to the server that was identified to respond to this
 * request
 */
void	Response::sendResponse(void)
{
	std::ifstream	file;

	if (this->_state == PENDING)
	{
		this->_fileLength = this->getFileSize(this->_filePath);
		if (this->_fullResponse.empty()) // here we check whether response was already prepared by a CGI script
		{
			//OPEN FILE THAT WILL BE SENT
			if (!this->_filePath.empty())
			{
				file.open(this->_filePath, std::ifstream::in | std::ifstream::binary);
				if (!file.is_open()) // MOVE UP 
				{
					this->_statusCode = FORBIDDEN; // SHOULD BE DIFFERENT
					throw std::ios_base::failure("Error when opening a file"); // LOOK INTO THIS, PROBABLY STATUSCODE INTERNAL_SERVER_ERROR (?)
				}
			}
			prepareFirstLine();
			prepareHeaders(this->_location->getRoot());
			if (this->_statusCode > 199 && this->_statusCode != DELETED && this->_statusCode != 304)
				prepareContent(file);
			if (!this->_filePath.empty())
			{
				file.close();
			}
		}
		this->_state = SENDING;
	}
	ssize_t bytesSent;
	ssize_t chunkSize = std::min(_fullResponse.length(), static_cast<size_t>(MAXLINE));
	// std::cout << "Chunksize is " << _fullResponse.length() << " or " << static_cast<size_t>(MAXLINE) << std::endl;
	if (this->_state == SENDING) // THIS SHOULD GO TO A SEPARATE FUNCTION, CALLED FROM HANDLERESPONSE
	{
		std::cerr << "[sendResponse] SENDING to fd " << this->_req.getConnFD() << std::endl;
		bytesSent = send(this->_req.getConnFD(), this->_fullResponse.c_str(), chunkSize, 0);
		if (bytesSent < 0)
			std::cout << "BytesSent error, send 500 internal error" << std::endl;
			//INTERNAL_SERVER_ERROR STATUSCODE
		_fullResponse.erase(0, bytesSent);
		if (_fullResponse.size() == 0 || bytesSent == 0)
			this->_state = DONE;
		std::cout << "State is " << this->_state << ", bytesSent = " << bytesSent << ", response leftover size is " << _fullResponse.size() << ", chunkSize = " << chunkSize << std::endl;
	}
	// BOUNCE CLIENT WHEN: INTERNAL_SERVER_ERROR
}

void	Response::prepareTargetURI(Server const & server)
{
	std::string	targetUri = this->_req.getTarget().substr(0, \
		this->_req.getTarget().find_first_of('?'));
	while (!this->_isReady)
	{
		try 
		{
			this->_location = findLocationMatch(targetUri, \
				server.getLocations());
			if (targetUri[targetUri.length() - 1] == '/')
			{
				if (this->_req.getMethod() == "GET" && \
					!this->_location->getIndexes().empty())
					targetUri = findIndexPage(this->_location);
				else 
				{
					// HERE COMES THE AUTOINDEX CODE
					// tbd what to return if a directory is requested and there is no index and autoindex is off
					// hier message vullen en statuscode en IETS in de filepath zetten OF JUIST NIET?
					this->_message = createAutoindex();	// DM: perhaps move autoindex to the end?
					this->_statusCode = NOT_FOUND;
					// this->_filePath = ""; // REMOVE
					this->_isReady = true;
				}
			}
			else
			{
				this->extractPathInfo(targetUri);
				this->_filePath = this->_location->getRoot() + targetUri;
				if (!hasReadPermission(this->_filePath))
					this->_statusCode = NOT_FOUND;
				this->_isReady = true;
			}
		}
		catch(const std::ios_base::failure & f)
		{
			std::cerr << "IOS exception caught (something went wrong with ";
			std::cerr << "opening the file " << this->_filePath << "): ";
			std::cerr << f.what() << std::endl;
			this->_statusCode = NOT_FOUND;
		}
		catch(const std::range_error &re)
		{
			std::cerr << "Range exception caught: (no location match found ";
			std::cerr << "for target " << this->_req.getTarget() << "): ";
			std::cerr << re.what() << std::endl;
			this->_statusCode = INTERNAL_SERVER_ERROR;
		}
	}
}

void	Response::identifyErrorPage(Server const & server)
{
	std::map<int, std::string> const & errorPages = \
		this->_location->getErrorPages();
	std::string	targetUri;

	std::cerr << "[identifyErrorPage] ";
	try // checking whether there is a defined error page for this error code in this location
	{
		targetUri = errorPages.at(this->_statusCode);
	}
	catch(const std::out_of_range& oor) // if not, using the default one
	{
		targetUri = DEFAULT_ERROR_PAGE;
	}
	this->_isReady = false;
	
	while (!this->_isReady)
	{
		std::cerr << "targetUri is " << targetUri;
		this->_location = findLocationMatch(targetUri, server.getLocations());
		std::cerr << ", location is " << this->_location->getMatch();
		this->_filePath = this->_location->getRoot() + targetUri;
		std::cerr << ", filePath is " << this->_filePath << std::endl;
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

/**
 * @brief finds a location match for the target URI
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance 
 * that is either the exact match for the target, if available, or the 
 * closest one.
 */
std::vector<Location>::const_iterator \
	Response::findLocationMatch(std::string target, \
	std::vector<Location> const & locations)
{
	std::vector<Location>::const_iterator	itLoc;

	itLoc = findExactLocationMatch(target, locations);
	if (itLoc == locations.end())
		itLoc = findWildcardLocationMatch(target, locations);
	if (itLoc == locations.end())
		itLoc = findClosestLocationMatch(target, locations);
	if (itLoc == locations.end())
	{
		this->_statusCode = INTERNAL_SERVER_ERROR;
		throw(std::range_error("No location match")); // further handle
	}
	return (itLoc);
}

/**
 * @brief iterates over locations and identifies the location with the exact
 * match for the target URI, if available
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance 
 * that is the exact match for the target. If no exact match was found, an 
 * iterator pointing to the end of the locations vector is returned.
 */
std::vector<Location>::const_iterator	\
	Response::findExactLocationMatch(std::string target, \
	std::vector<Location> const & locations)
{
	std::vector<Location>::const_iterator it;
	
	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() != "=")
			continue;
		if (target == it->getMatch())
			return (it);
	}
	return (it);
}

std::vector<Location>::const_iterator	\
	Response::findWildcardLocationMatch(std::string target, \
	std::vector<Location> const & locations)
{
	std::vector<Location>::const_iterator	it;
	std::vector<std::string>				targetSplit;

	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() != "=" && it->getMatch().find('*') == 0)
		{
			std::string	needle = it->getMatch().substr(1);
			this->splitUri(target, targetSplit);
			for (size_t i = 0; i < targetSplit.size(); i++)
			{
				if (targetSplit[i].find(needle) == \
					targetSplit[i].length() - needle.length())
				{
					std::cerr << "Target is " << target << ", targetSplit[";
					std::cerr << i << "] = " << targetSplit[i];
					std::cerr << ", needle = " << needle << std::endl;
					return (it);
				}
			}
		}
	}
	return (it);
}

/**
 * @brief iterates over locations and identifies the location with the greatest
 * overlap with the target URI. To enable the comparison, the target and match 
 * URI are split into elements by calling the function splitURI().
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance 
 * that is the closest match for the target. If no location is found, iterator 
 * to the end of the locations vector is returned.
 */
std::vector<Location>::const_iterator	\
	Response::findClosestLocationMatch(std::string target, \
	std::vector<Location> const & locations)
{
	size_t									overlap = 0;
	std::vector<std::string>				targetSplit;
	std::vector<Location>::const_iterator	longest = locations.end();
	size_t									idx;
	size_t									len = 0;

	this->splitUri(target, targetSplit);
	// for (std::vector<std::string>::iterator i = targetSplit.begin(); \
	// 	i != targetSplit.end(); i++) // JMA: what does this do? Can it go?
	for (auto it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() == "=")
			continue;
		std::vector<std::string>	matchSplit;
		this->splitUri(it->getMatch(), matchSplit);
		len = matchSplit.size();
		for (idx = 0; idx < len; idx++)
		{
			if (idx == targetSplit.size() || \
				targetSplit[idx] != matchSplit[idx])
			{
				idx = 0;
				break;
			}
		}
		// std::cout << "checking match: " << it->getMatch() << "\tidx: ";
		// std::cout << idx << std::endl;
		if (idx > overlap)
		{
			// std::cout << "better match found: " << it->getMatch();
			// std::cout << "overlap: " << overlap << std::endl;
			overlap = idx;
			longest = it;
		}
		matchSplit.clear();
	}
	return (longest);
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
std::string	\
	Response::findIndexPage(std::vector<Location>::const_iterator itLoc)
{
	std::string					filePath;
	std::vector<std::string>	indexes = itLoc->getIndexes();

	for (auto itIdx = indexes.begin(); itIdx != indexes.end(); itIdx++)
	{
		// std::cout<< "itIdx: [" << *itIdx << "]" << std::endl;
		filePath = itLoc->getRoot() + *itIdx;
		// std::cout<< "Filepath: [" << filePath << "]" << std::endl;

		if (access(filePath.c_str(), F_OK) == 0)
			return (*itIdx);
	}
	this->_statusCode = NOT_FOUND;
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
		std::cerr << "[pathInfo extraction] targetUri is " << targetUri;
		std::cerr << ", pathInfo is " << this->_pathInfo << std::endl;
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
	
	std::memset(responseBuffer, 0, RESPONSELINE + 1); // CHECK IF FAILED

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
	
	snprintf(responseBuffer, RESPONSELINE, "%s %d %s\r\n",	\
		this->_req.getProtocolVersion().c_str(), this->_statusCode, \
		responseMessage.c_str());
	
	printf("\n\nRESPONSE: [%s]\n\n", (char*)responseBuffer);
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
	char			responseBuffer[RESPONSELINE + 1];
	std::string		contentType;

	if (!this->_filePath.empty())
	{
		contentType = root == "data" ? \
		"image/" + this->_filePath.substr(this->_filePath.find_last_of('.') + 1, \
		std::string::npos) : "text/" + this->_filePath.substr(this->_filePath.find_last_of('.') + 1);
		std::memset(responseBuffer, 0, RESPONSELINE); // CHECK IF FAILED
		snprintf(responseBuffer, RESPONSELINE, "Content-Type: %s\r\n", contentType.c_str());
	}
	else if (this->_statusCode == NOT_FOUND && this->_location->getAutoindex() == 1)
	{
		// ELSE TEXT/HTML FOR AUTOINDEX, this should be checked and improved
		contentType = "text/html";
		std::memset(responseBuffer, 0, RESPONSELINE);
		snprintf(responseBuffer, RESPONSELINE, "Content-Type: %s\r\n", contentType.c_str());
	}

	if (this->_fileLength == 0 && this->_message.length() > 0)
		snprintf(responseBuffer, RESPONSELINE, "Content-Length: %zu\r\n\r\n", this->_message.length());
	else if (this->_location->getReturnLink().size() > 0)
		snprintf(responseBuffer, RESPONSELINE, "Content-Length: %zu\r\nLocation: %s\r\n\r\n", this->_message.size(), this->_location->getReturnLink().c_str());
	else
		snprintf(responseBuffer, RESPONSELINE, "Content-Length: %zu\r\n\r\n", this->_fileLength);

	// printf("\n\nFILEPATH: [%s]\n\n", this->_filePath.c_str());
	// printf("\n\nRESPONSE: [%s]\n\n", (char*)responseBuffer);
	// printf("\n\nCONTENT TYPE: [%s]\n\n", contentType.c_str());

	this->addToFullResponse(&responseBuffer[0], std::strlen(responseBuffer));
}

/**
 * @brief copy-pastes the content of the target file into the response buffer
 * in chunks of RESPONSELINE characters and sends each chunk to the client. 
 * After sending, the buffer is cleared with std::memset(). If the file cannot 
 * be opened, an exception is thrown.
 * 
 * @param response the buffer into which the response is written
 */
void	Response::prepareContent(std::ifstream	&file)
{
	std::string		body;
	
	// DM: add first check whether _filePath is empty, zo ja, plak message in body
	// also check response codes die geen body mogen hebben (dat er geen body komt)
	
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

/* UTILS */

/**
 * @brief finds out the size of a file
 * 
 * @param filePath path to the file, the size of which is to be determined
 * @return size_t indicating the size of the file (in bytes)
 */
size_t	Response::getFileSize(std::string filePath)
{
	std::ifstream	file;
	size_t			len;

	file.open(filePath, std::ifstream::binary | std::ifstream::ate);	
	if (file.is_open())
	{
		len = file.tellg();
		file.clear();
		file.close();
		return (len);
	}
	return (0);
}

/**
 * @brief splits an URI path into chunks (a chunk is either a '/' character or 
 * a string between two '/' characters). Ignores any potential query part 
 * (delimited by a '?' character)
 * 
 * @param uri the uri string to be split
 * @param chunks reference to a vector, in which the chunks are to be placed
 */
void	Response::splitUri(std::string const & uri, \
	std::vector<std::string> & chunks)
{
	size_t	begin = 0;
	size_t	end;

	while (begin < uri.length() && begin < uri.find_first_of('?'))
	{

		end = uri[begin];
		if (uri[begin] == '/') 
			end = uri.find_first_of('/', begin) + 1;
		else
			end = uri.find_first_of('/', begin);
		chunks.push_back(uri.substr(begin, end - begin));
		begin = end;
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
