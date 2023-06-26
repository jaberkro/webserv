#include "Response.hpp"

#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include "server.hpp"
#include <cstdio>

#include <vector>

Response::Response(Request req) : _req (req), _statusCode (200), _msgLength (0), \
_isReady (false) {}

Response::~Response(void) {}

Response::Response(Response &r) : _req (r.getRequest()), \
_statusCode (r.getStatusCode()), _msgLength (r.getMsgLength()), \
_isReady (r.getIsReady()) {}

Response &	Response::operator=(Response &)
{
	return (*this);
}


/**
 * @brief 
 * 
 * @return uint8_t* 
 */
void	Response::retrieveFile(uint8_t *response)
{
	std::ifstream	file;
	std::string		contentType;
	uint8_t			fileBuf[MAXLINE + 1];

	std::memset(fileBuf, 0, MAXLINE);

	contentType = "text/html";
	file.open(this->_filePath, std::ifstream::in);
	if (!file.is_open())
		throw std::ios_base::failure("Error when opening a file");
	file.read((char *)fileBuf, MAXLINE);
	snprintf((char *)response, MAXLINE, \
	"%s %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n\r\n%s", \
	this->_req.getProtocolVersion().c_str(), this->_statusCode, this->_reason.c_str(), \
	contentType.c_str(), std::strlen((char *)fileBuf), (char *)fileBuf);
	this->_msgLength = std::strlen((char *)response);
	file.close();
}

/**
 * @brief 
 * 
 * @return uint8_t* 
 */
void	Response::retrieveImg(uint8_t *response)
{
	std::ifstream				file;
	size_t 						i = 0;
	std::vector<std::string>	images = {".jpg", ".jpeg", ".png"};
	size_t						lengthFile;
	std::string					contentType;
	std::filebuf				*imgBuf = file.rdbuf();
	
	while (i < images.size() && this->_filePath.find(images[i]) == std::string::npos)
		std::cout << "Finding " << images[i++] << " in " << this->_filePath << std::endl;
	if (i < images.size())
	 	contentType = "image/" + images[i].substr(1,std::string::npos);
	else
		std::cout << "Bad request - unknown format" << std::endl;

	lengthFile = this->getFileSize(this->_filePath);
	file.open(this->_filePath, std::ifstream::in | std::ifstream::binary);
	if (!file.is_open())
		throw std::ios_base::failure("Error when opening a file");

	snprintf((char *)response, MAXLINE, \
	"%s %d %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n\r\n", \
	this->_req.getProtocolVersion().c_str(), this->_statusCode, this->_reason.c_str(), \
	contentType.c_str(), lengthFile);
	this->_msgLength = std::strlen((char *)response);
	imgBuf = file.rdbuf();
	for ( size_t i = 0; i < lengthFile; i++ )
		response[this->_msgLength + i] = imgBuf->sbumpc();
	this->_msgLength += lengthFile;
	file.close();
}

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

std::vector<Location>::iterator Response::findMatch(std::string target, std::vector<Location> & locations)
{
	std::vector<Location>::iterator	itLoc;

	itLoc = findExactMatch(target, locations);
		if (itLoc == locations.end())
	itLoc = findClosestMatch(target, locations);
	return (itLoc);
}

void	Response::prepareGetResponse(uint8_t *response, std::vector<Location> & locations)
{
	std::vector<Location>::iterator	itLoc;
	std::string						targetUri = \
	this->_req.getTarget().substr(0, this->_req.getTarget().find_first_of('?'));

	while (!this->_isReady)
	{
		itLoc = findMatch(targetUri, locations);
		if (!itLoc->getIndex().empty())
			this->_filePath = itLoc->getRoot() + targetUri + itLoc->getIndex();
		else
			this->_filePath = itLoc->getRoot() + targetUri;
		try 
		{
			if (itLoc->getRoot() == "data/www")
				this->retrieveFile(response);
			else
				this->retrieveImg(response);
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
 * @brief 
 * 
 * @param sb 
 * @return uint8_t* 
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
 * @brief 
 * 
 * @param filePath 
 * @return size_t 
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
 * @brief 
 * 
 * @return size_t 
 */
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

void	Response::splitUri(std::string const & uri, std::vector<std::string> & chunks)
{
	size_t	begin = 0;
	size_t	end;

	while (begin < std::string::npos)
	{
		end = uri[begin] == '/' ? uri.find_first_of('/', begin) + 1 : uri.find_first_of('/', begin);
		chunks.push_back(uri.substr(begin, end - begin));
		begin = end;
	}
}
