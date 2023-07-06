#include "Response.hpp"

#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include "server.hpp"
#include <cstdio>
#include <unistd.h>

#include <vector>

std::map<int, std::string> 	Response::_responseCodes = 
{
	{200, "OK"},
	{400, "Bad Request"},
	{404, "Not Found"}
};

Response::Response(Request req) : \
_req (req), \
_statusCode (req.getStatusCode()), \
_fileLength (0), \
_isReady (false) {}

Response::~Response(void) {}

Response::Response(Response &r) : \
_req (r.getRequest()), \
_statusCode (r.getStatusCode()), \
_fileLength (r.getFileLength()), \
_isReady (r.getIsReady()) {}

Response &	Response::operator=(Response &r)
{
	this->_req = r.getRequest();
	this->_statusCode = r.getStatusCode();
	this->_fileLength = r.getFileLength();
	this->_isReady = r.getIsReady();
	return (*this);
}

/**
 * @brief retrieves a file or throws an exception
 * 
 * @param response the buffer into which the response is written
 */
void	Response::retrieveFile(uint8_t *response, std::string const & root)
{
	
	this->_fileLength = this->getFileSize(this->_filePath);
	if (access(this->_filePath.c_str(), F_OK | R_OK) < 0)
	{
		this->_statusCode = 440;
		throw std::ios_base::failure("File not found");
	}
	sendFirstLine(response);
	sendHeaders(response, root);
	sendContentInChunks(response);
}

/**
 * @brief sends the content of the target file in chunks of CHUNK_SIZE characters
 * 
 * @param response the buffer into which the response is written
 */
void	Response::sendContentInChunks(uint8_t *response)
{
	std::ifstream	file;
	std::filebuf	*fileBuf;
	
	file.open(this->_filePath, std::ifstream::in | std::ifstream::binary);
	if (!file.is_open())
		throw std::ios_base::failure("Error when opening a file");
	fileBuf = file.rdbuf();
	for ( size_t i = 0; i < this->_fileLength; i++ )
	{
		
		// std::cout << "i is " << i << ", fileLen is " << this->_fileLength << std::endl;
		response[i % CHUNK_SIZE] = fileBuf->sbumpc();
		if (i % CHUNK_SIZE == CHUNK_SIZE - 1)
		{
			// std::cout << "i is " << i << ", sending \"" << response << "\"" << std::endl;
			send(this->_req.getConnFD(), (char*)response, CHUNK_SIZE, 0);
			std::memset(response, 0, CHUNK_SIZE);
		}
	}
	send(this->_req.getConnFD(), (char*)response, this->_fileLength % CHUNK_SIZE, 0);
	std::memset(response, 0, CHUNK_SIZE);
	file.close();
}

/**
 * @brief sends the first line of the response and the headers to the client
 * 
 * @param response the buffer into which the response is written
 * @param root the root directory (from the location block; used to distinguish
 * between the file types that are to be returned - to determine Content Type)
 */
void	Response::sendFirstLine(uint8_t *response)
{
	snprintf((char *)response, MAXLINE, \
	"%s %d %s\r\n",	this->_req.getProtocolVersion().c_str(), this->_statusCode, this->_responseCodes.at(this->_statusCode).c_str());
	send(this->_req.getConnFD(), (char*)response, std::strlen((char *)response), 0);
	std::memset(response, 0, MAXLINE);
}

void	Response::sendHeaders(uint8_t *response, std::string const & root)
{
	std::string		contentType = root.compare("data") == 0 ? \
	"image/" + this->_filePath.substr(this->_filePath.find_last_of('.') + 1, std::string::npos) : "text/html";

	std::cout << "Content Type is " << contentType << std::endl;
	snprintf((char *)response, MAXLINE, \
	"Content-Type: %s\r\nContent-Length: %zu\r\n\r\n", contentType.c_str(), this->_fileLength);
	send(this->_req.getConnFD(), (char*)response, std::strlen((char *)response), 0);
	std::memset(response, 0, MAXLINE);
}


/**
 * @brief iterates over locations and identifies the location with the greatest
 * overlap with the target URI
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance that 
 * is the closest match for the target. 
 */
std::vector<Location>::iterator	Response::findClosestMatch(std::string target, \
std::vector<Location> & locations)
{
	size_t	overlap = 0;
	std::vector<std::string>	targetSplit;
	std::vector<Location>::iterator longest;
	std::vector<Location>::iterator it;
	size_t	idx = 0;
	size_t	len = 0;

	this->splitUri(target, targetSplit);
	for (std::vector<std::string>::iterator i = targetSplit.begin(); i != targetSplit.end(); i++)
	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() == "=")
			continue;
		std::vector<std::string>	matchSplit;
		this->splitUri(it->getMatch(), matchSplit);
		len = std::min(targetSplit.size(), matchSplit.size());
		while (idx < len && targetSplit[idx].compare(matchSplit[idx]) == 0)
			idx++;
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
 * @brief iterates over locations and identifies the location with the exact
 * match for the target URI, if available
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance that 
 * is the exact match for the target. If no exact match was found, an iterator
 * pointing to the end of the vector is returned.
 */
std::vector<Location>::iterator	Response::findExactMatch(std::string target, std::vector<Location> & locations)
{
	std::vector<Location>::iterator it;
	
	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() != "=")
			continue;
		if (target.compare(it->getMatch()) == 0)
			return (it);
	}
	return (it);
}

/**
 * @brief finds a match for the target URI
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance that 
 * is either the exact match for the target, if available, or the closest one.
 */
std::vector<Location>::iterator Response::findMatch(std::string target, std::vector<Location> & locations)
{
	std::vector<Location>::iterator	itLoc;

	itLoc = findExactMatch(target, locations);
	if (itLoc == locations.end())
		itLoc = findClosestMatch(target, locations);
	return (itLoc);
}

std::string	Response::findIndex(std::vector<Location>::iterator itLoc)
{
	for (std::vector<std::string>::iterator itIdx = itLoc->getIndexes().begin(); \
	itIdx != itLoc->getIndexes().end(); itIdx++)
	{
		// if you find the file, return its name
		if (access((*itIdx).c_str(), F_OK) == 0)
			return (*itIdx);
	}
	this->_statusCode = 440;
	throw std::ios_base::failure("Index file not found");
}

/**
 * @brief contains a loop, in which (i) the relevant location is identified, (ii)
 * if necessary, internal redirect is performed, leading to another identification
 * of the relevant location, and (iii) once the definitive target has been found,
 * the suitable function is called for preparation of the response.
 * 
 * @param response the buffer in which the response is composed
 * @param locations the vector of Location instances containing the configuration
 * of the server's locations
 */
void	Response::prepareResponseGET(uint8_t *response, std::vector<Location> & locations)
{
	std::vector<Location>::iterator	itLoc;
	std::string						targetUri = \
	this->_req.getTarget().substr(0, this->_req.getTarget().find_first_of('?'));

	if (this->_statusCode == 400)
		sendFirstLine(response);
	else
		while (!this->_isReady)
		{
			itLoc = findMatch(targetUri, locations);

			try 
			{
				if (!itLoc->getIndexes().empty()) // only look for index if target ends by "/" ?
				{
					targetUri = findIndex(itLoc);
					continue;
				}
				else
				{
					this->_filePath = itLoc->getRoot() + targetUri;
					this->retrieveFile(response, itLoc->getRoot());
					this->_isReady = true;
				}
			}
			catch(const std::ios_base::failure & f)
			{
				std::cout << "Exception caught: ";
				std::cout << f.what() << std::endl;
				try
				{
					targetUri = itLoc->getErrorPages().at(this->_statusCode);
				}
				catch(const std::out_of_range& oor)
				{
					std::cerr << "No error page specified: " << oor.what() << std::endl;
					targetUri = "/defaultError.html";
				}
			}
		}
}

/**
 * @brief at the moment, this function primarily creates a dummy structure for 
 * testing; to be refactored after integration with the config part
 * 
 * @return uint8_t* containing the response to be passed to the client
 */
void	Response::createResponse()
{
	uint8_t					*response = new uint8_t[MAXLINE + 1];
	std::vector<Location>	locations;
	Location				a;
	Location				b;
	Location				c;


	a.setMatch("/");
	a.setModifier("=");
	a.addIndex("index.html");
	a.setRoot("data/www");
	b.setMatch("/");
	b.setModifier("");
	b.setRoot("data/www");
	b.addErrorPage(404, "/404.html");
	c.setMatch("/images");
	c.setModifier("");
	c.setRoot("data");
	c.addErrorPage(404, "/404.html");
	locations.push_back(a);
	locations.push_back(b);
	locations.push_back(c);
	
	std::memset(response, 0, MAXLINE);
	if (this->_req.getMethod() == "GET")
		prepareResponseGET(response, locations);
}

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

	while (begin < std::string::npos && begin < uri.find_first_of('?'))
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

