#include "Response.hpp"

#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include "Webserver.hpp"
#include <cstdio>
#include <unistd.h>

#include <vector>

/**
 * @brief static variable _responseCodes is a map containing all reasons that 
 * belong to various status codes. These are used in the response sent to the 
 * client.
 */
std::map<int, std::string> 	Response::_responseCodes = 
{
	{OK, "OK"},
	{BAD_REQUEST, "Bad Request"},
	{FORBIDDEN, "Forbidden"},
	{NOT_FOUND, "Not Found"},
	{INTERNAL_SERVER_ERROR, "Internal Server Error"}
};

/**
 * @brief Construct a new Response:: Response object
 * 
 * @param req reference to a Request instance
 */
Response::Response(Request & req) : \
_req (req), \
_statusCode (req.getStatusCode()), \
_fileLength (0), \
_isReady (false) {}

/**
 * @brief Destroy the Response:: Response object
 * 
 */
Response::~Response(void) {}

/**
 * @brief Construct a new Response:: Response object
 * 
 * @param r reference to a Response instance to be copied
 */
Response::Response(Response &r) : \
_req (r.getRequest()), \
_statusCode (r.getStatusCode()), \
_fileLength (r.getFileLength()), \
_isReady (r.getIsReady()) {}

/**
 * @brief Response copy assignment operator
 * 
 * @param r reference to a Response instance to be copied
 * @return Response& 
 */
Response &	Response::operator=(Response & r)
{
	this->_req = r.getRequest();
	this->_statusCode = r.getStatusCode();
	this->_fileLength = r.getFileLength();
	this->_isReady = r.getIsReady();
	return (*this);
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
void	Response::prepareResponseGET(Server const & server)
{
	std::vector<Location>::const_iterator	itLoc;
	std::string								targetUri = \
	this->_req.getTarget().substr(0, this->_req.getTarget().find_first_of('?'));
	int	rounds = 0;
	
	if (this->_req.getMethod() == "")
		close(this->_req.getConnFD());
	else if (this->_req.getMethod() != "GET")
		std::cout << "I cannot handle the \"" << this->_req.getMethod() \
		<< "\" method just yet, sorry!" << std::endl;
	else
		while (!this->_isReady && rounds++ < 6)
		{
			// std::cout << "Target Uri is " << targetUri << std::endl;
			try 
			{
				itLoc = findMatch(targetUri, server.getLocations());
				// std::cout << "Matching location found: " << itLoc->getMatch() << std::endl;
				if (targetUri[targetUri.length() - 1] == '/' && !itLoc->getIndexes().empty())
				{
					targetUri = findIndexPage(itLoc);
					continue;
				}
				else
				{
					this->_filePath = itLoc->getRoot() + targetUri;
					// std::cout << "File path is " << this->_filePath << std::endl;
					this->retrieveFile(itLoc->getRoot());
					this->_isReady = true;
				}
			}
			catch(const std::ios_base::failure & f)
			{
				std::cerr << "IOS exception caught: ";
				std::cerr << f.what() << std::endl;
				targetUri = identifyErrorPage(itLoc);
			}
			catch(const std::range_error &re)
			{
				std::cerr << "Range exception caught: ";
				std::cerr << re.what() << std::endl;
				
				// TO BE ADDED: try to find a corresponding error page in the SERVER block;
				targetUri = "data/www/defaultError.html";
			}
			if (rounds == 6)
				std::cout << "--> loop ended after 6 rounds <--" << std::endl;
		}
}

/**
 * @brief Returns the filename of the error page corresponding to the status code.
 * If no error page was provided in the config file for a specific status code,
 * the filename of the default error page is returned instead.
 * 
 * @param itLoc iterator to the relevant Location object that is the exact or 
 * closest match for the target
 * @return std::string filename of the error page to be returned
 */
std::string	Response::identifyErrorPage(std::vector<Location>::const_iterator itLoc)
{
	try
	{
		return (itLoc->getErrorPages().at(this->_statusCode));
	}
	catch(const std::out_of_range& oor)
	{
		// std::cerr << "No custom error page found" << std::endl;
		return("/defaultError.html");
	}
}

/**
 * @brief finds a location match for the target URI
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance that 
 * is either the exact match for the target, if available, or the closest one.
 */
std::vector<Location>::const_iterator Response::findMatch(std::string target, \
std::vector<Location> const & locations)
{
	std::vector<Location>::const_iterator	itLoc;

	itLoc = findExactMatch(target, locations);
	if (itLoc == locations.end())
		itLoc = findClosestMatch(target, locations);
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
 * @return std::vector<Location>::iterator pointing to a Location instance that 
 * is the exact match for the target. If no exact match was found, an iterator
 * pointing to the end of the locations vector is returned.
 */
std::vector<Location>::const_iterator	Response::findExactMatch(std::string target, \
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

/**
 * @brief iterates over locations and identifies the location with the greatest
 * overlap with the target URI. To enable the comparison, the target and match URI 
 * are split into elements by calling the function splitURI().
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance that 
 * is the closest match for the target. If no location is found, iterator to the
 * end of the locations vector is returned.
 */
std::vector<Location>::const_iterator	Response::findClosestMatch(std::string target, \
std::vector<Location> const & locations)
{
	size_t									overlap = 0;
	std::vector<std::string>				targetSplit;
	std::vector<Location>::const_iterator	longest = locations.end();
	size_t									idx;
	size_t									len = 0;

	this->splitUri(target, targetSplit);
	for (std::vector<std::string>::iterator i = targetSplit.begin(); i != targetSplit.end(); i++)
	for (auto it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() == "=")
			continue;
		std::vector<std::string>	matchSplit;
		this->splitUri(it->getMatch(), matchSplit);
		len = matchSplit.size();
		for (idx = 0; idx < len; idx++)
		{
			if (idx == targetSplit.size() || targetSplit[idx] != matchSplit[idx])
			{
				idx = 0;
				break;
			}
		}
		if (idx > overlap)
		{
			overlap = idx;
			longest = it;
		}
		matchSplit.clear();
	}
	return (longest);
}

/**
 * @brief loops through the index page filenames provided in the config file for 
 * a particular location. For each one, it constructs the path to the file and 
 * tries to locate the file. If successful, it returns the filename of that index
 * page. If none of the files is found, the status code is set to 404 (Not Found) 
 * and an exception is thrown.
 * 
 * @param itLoc iterator to the relevant Location object
 * @return std::string - the filename of the index corresponding to the location. 
 */
std::string	Response::findIndexPage(std::vector<Location>::const_iterator itLoc)
{
	std::string							filePath;
	std::vector<std::string>			indexes = itLoc->getIndexes();

	for (auto itIdx = indexes.begin(); itIdx != indexes.end(); itIdx++)
	{
		filePath = itLoc->getRoot() + *itIdx;
		if (access(filePath.c_str(), F_OK) == 0)
			return (*itIdx);
	}
	this->_statusCode = NOT_FOUND;
	throw std::ios_base::failure("Index file not found");
}

/**
 * @brief retrieves a file or throws an exception, if file is not found.
 * 
 * @param response the buffer into which the response is written
 */
void	Response::retrieveFile(std::string const & root)
{
	
	this->_fileLength = this->getFileSize(this->_filePath);
	if (access(this->_filePath.c_str(), F_OK | R_OK) < 0)
	{
		this->_statusCode = NOT_FOUND;
		throw std::ios_base::failure("File not found");
	}
	sendFirstLine();
	sendHeaders(root);
	sendContentInChunks();
}


/**
 * @brief composes the first line of a response, writing it into the response buffer.
 * After sending the response, it clears the buffer with std::memset().
 * 
 * @param response the buffer into which the response is written
 */
void	Response::sendFirstLine(void)
{
	uint8_t	response[MAXLINE + 1];
	
	std::memset(response, 0, MAXLINE);
	snprintf((char *)response, MAXLINE, \
	"%s %d %s\r\n",	this->_req.getProtocolVersion().c_str(), this->_statusCode, \
	this->_responseCodes.at(this->_statusCode).c_str());
	send(this->_req.getConnFD(), (char*)response, std::strlen((char *)response), 0);
}

/**
 * @brief composes the headers of a response, writing them into the response buffer.
 * After sending, it clears the buffer with std::memset().
 * 
 * @param response the buffer into which the response is written
 * @param root the root directory (from the location block; used to distinguish
 * between the file types that are to be returned - to determine Content Type)
 */
void	Response::sendHeaders(std::string const & root)
{
	uint8_t	response[MAXLINE + 1];
	std::string		contentType = root == "data" ? \
	"image/" + this->_filePath.substr(this->_filePath.find_last_of('.') + 1, \
	std::string::npos) : "text/html";

	std::memset(response, 0, MAXLINE);
	snprintf((char *)response, MAXLINE, \
	"Content-Type: %s\r\nContent-Length: %zu\r\n\r\n", contentType.c_str(), this->_fileLength);
	send(this->_req.getConnFD(), (char*)response, std::strlen((char *)response), 0);
}


/**
 * @brief copy-pastes the content of the target file into the response buffer
 * in chunks of CHUNK_SIZE characters and sends each chunk to the client. After sending, 
 * the buffer is cleared with std::memset(). If the file cannot be opened, an
 * exception is thrown.
 * 
 * @param response the buffer into which the response is written
 */
void	Response::sendContentInChunks(void)
{
	uint8_t			response[CHUNK_SIZE + 1];
	std::ifstream	file;
	std::filebuf	*fileBuf;
	
	std::memset(response, 0, CHUNK_SIZE);
	file.open(this->_filePath, std::ifstream::in | std::ifstream::binary);
	if (!file.is_open())
	{
		this->_statusCode = FORBIDDEN;
		throw std::ios_base::failure("Error when opening a file");
	}
	fileBuf = file.rdbuf();
	for ( size_t i = 0; i < this->_fileLength; i++ )
	{
		
		response[i % CHUNK_SIZE] = fileBuf->sbumpc();
		if (i % CHUNK_SIZE == CHUNK_SIZE - 1)
		{
			send(this->_req.getConnFD(), (char*)response, CHUNK_SIZE, 0);
			std::memset(response, 0, CHUNK_SIZE);
		}
	}
	send(this->_req.getConnFD(), (char*)response, this->_fileLength % CHUNK_SIZE, 0);
	std::memset(response, 0, CHUNK_SIZE);
	send(this->_req.getConnFD(), (char*)response, 0, 0);
	file.close();
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
void	Response::splitUri(std::string const & uri, std::vector<std::string> & chunks)
{
	size_t	begin = 0;
	size_t	end;

	while (begin < uri.length() && begin < uri.find_first_of('?'))
	{
		end = uri[begin] == '/' ? uri.find_first_of('/', begin) + 1 : uri.find_first_of('/', begin);
		chunks.push_back(uri.substr(begin, end - begin));
		begin = end;
	}
}

/* GETTERS */

size_t	Response::getFileLength( void ) const
{
	return (this->_fileLength);
}

std::string	& Response::getFilePath(void)
{
	return (this->_filePath);
}

bool	Response::getIsReady(void)
{
	return (this->_isReady);
}


Request &	Response::getRequest(void)
{
	return (this->_req);
}

int	Response::getStatusCode(void)
{
	return (this->_statusCode);
}

/* TO BE DELETED */
void	Response::printResponse(void) const
{
	std::cout << "***RESPONSE***" << std::endl;
	std::cout << "\tStatus code: " << this->_statusCode << std::endl;
	std::cout << "\tReason: " << this->_responseCodes[this->_statusCode] << std::endl;
	std::cout << "\tProtocol Version: " << this->_req.getProtocolVersion() << std::endl;
	std::cout << "\tConnection FD: " << this->_req.getConnFD() << std::endl;
	std::cout << "******" << std::endl;
}