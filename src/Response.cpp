#include "Response.hpp"

#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include "server.hpp"
#include <cstdio>

#include <vector>

Response::Response(Request req) : \
_req (req), \
_statusCode (200), \
_msgLength (0), \
_isReady (false) {}

Response::~Response(void) {}

Response::Response(Response &r) : \
_req (r.getRequest()), \
_statusCode (r.getStatusCode()), \
_msgLength (r.getMsgLength()), \
_isReady (r.getIsReady()) {}

Response &	Response::operator=(Response &r)
{
	this->_req = r.getRequest();
	this->_statusCode = r.getStatusCode();
	this->_msgLength = r.getMsgLength();
	this->_isReady = r.getIsReady();
	return (*this);
}

/**
 * @brief retrieves a file or throws an exception
 * 
 * @param response the buffer into which the response is written
 */
void	Response::retrieveFile(uint8_t *response)
{
	std::ifstream				file;
	size_t						lengthFile;
	std::string					contentType;
	std::filebuf				*imgBuf = file.rdbuf();
	
	lengthFile = this->getFileSize(this->_filePath);
	// here do access to find whether the file exists and whether we have permissions
	file.open(this->_filePath, std::ifstream::in | std::ifstream::binary);
	if (!file.is_open())
		throw std::ios_base::failure("Error when opening a file");

	snprintf((char *)response, MAXLINE, \
	"%s %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n\r\n", \
	this->_req.getProtocolVersion().c_str(), this->_statusCode, this->_reason.c_str(), \
	contentType.c_str(), lengthFile);
	this->_msgLength = std::strlen((char *)response);
	imgBuf = file.rdbuf();
	for ( size_t i = 0; i < lengthFile && i < MAXLINE - this->_msgLength; i++ )
		response[this->_msgLength + i] = imgBuf->sbumpc();
	this->_msgLength += lengthFile;
	file.close();
	
	send(this->_req.getConnFD(), (char*)response, this->getMsgLength(), 0);
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
void	Response::prepareGetResponse(uint8_t *response, std::vector<Location> & locations)
{
	std::vector<Location>::iterator	itLoc;
	std::string						targetUri = \
	this->_req.getTarget().substr(0, this->_req.getTarget().find_first_of('?'));

	while (!this->_isReady)
	{
		itLoc = findMatch(targetUri, locations);
		if (!itLoc->getIndex().empty())
		{
			// std::cout << "Root is " << itLoc->getRoot() << ", targetURI is " << targetUri << ", index is " << itLoc->getIndex() << std::endl;
			targetUri = targetUri + itLoc->getIndex();
			// std::cout << "Target is now " << targetUri << std::endl;
			continue;
		}
		else
		{
			this->_filePath = itLoc->getRoot() + targetUri;
			// std::cout << "File Path is " << this->_filePath << std::endl;
		}
		try 
		{
			this->retrieveFile(response);
			this->_isReady = true;
		}
		catch(const std::ios_base::failure & f)
		{
			// create a function getErrorPage()
			const std::vector<std::pair<int,std::string>>  &	errorPages = itLoc->getErrorPages(); 
			for (auto it = errorPages.begin(); it != errorPages.end(); it++)
			{
				// to be edited - more error codes to be included
				if (it->first == 404)
				{
					targetUri = it->second;
					this->_statusCode = 404;
					this->_filePath.clear();
				}
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
uint8_t *	Response::createResponse()
{
	uint8_t					*response = new uint8_t[MAXLINE + 1];
	std::vector<Location>	locations;
	Location				a;
	Location				b;
	Location				c;


	a.setMatch("/");
	a.setModifier("=");
	a.setIndex("index.html");
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
		prepareGetResponse(response, locations);
	return (response);
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
size_t	Response::getMsgLength( void ) const
{
	return (this->_msgLength);
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

