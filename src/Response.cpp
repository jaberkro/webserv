#include "Response.hpp"

#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include "server.hpp"
#include <cstdio>

#include <vector>
#include <map>

Response::Response(Request req) : _req (req), _errorCode (0) {}

Response::~Response(void) {}

Response::Response(Response &r) : _req (r.getRequest()), _errorCode (r.getErrorCode()) {}

Response &	Response::operator=(Response &)
{
	return (*this);
}



/**
 * @brief 
 * 
 * @return uint8_t* 
 */
uint8_t *	Response::createResponseHtml(void)
{
	std::string		pathToFile;
	std::ifstream	file;
	std::string		contentType;
	uint8_t			fileBuf[MAXLINE + 1];
	uint8_t			*response = new uint8_t[MAXLINE + 1];

	std::memset(fileBuf, 0, MAXLINE);
	std::memset(response, 0, MAXLINE);

	pathToFile = this->_req.getTarget() == "/" ? "data/www/index.html" : "data/www" + this->_req.getTarget();
	contentType = "text/html";
	file.open(pathToFile);
	file.read((char *)fileBuf, MAXLINE);

	snprintf((char *)response, MAXLINE, \
	"%s 200 OK\r\nContent-Type: %s\r\nContent-Length: %zu\r\n\r\n%s", \
	this->_req.getProtocolVersion().c_str(), contentType.c_str(), std::strlen((char *)fileBuf), (char *)fileBuf);
	this->_msgLength = std::strlen((char *)response);
	file.close();
	return (response);
}

/**
 * @brief 
 * 
 * @return uint8_t* 
 */
uint8_t *	Response::createResponseImg(void)
{
	std::string		pathToFile;
	std::ifstream	file;
	size_t			lengthFile;
	std::string		contentType;
	std::filebuf	*imgBuf = file.rdbuf();
	uint8_t			*response = new uint8_t[MAXLINE + 1];
	
	std::memset(response, 0, MAXLINE);
	pathToFile = "data" + this->_req.getTarget();
	contentType = "image/jpg";


	try
	{
		lengthFile = this->getFileSize(pathToFile);
		file.open(pathToFile, std::ifstream::in | std::ifstream::binary);
	}
	catch(const std::ios_base::failure& f)
	{
		std::cerr << "Caught an exception: " << f.what() << std::endl;
		snprintf((char *)response, MAXLINE, "%s 404 Not Found", this->_req.getProtocolVersion().c_str());
	}
	snprintf((char *)response, MAXLINE, \
	"%s 200 OK\r\nContent-Type: %s\r\nContent-Length: %zu\r\n\r\n", \
	this->_req.getProtocolVersion().c_str(), contentType.c_str(), lengthFile);
	this->_msgLength = std::strlen((char *)response);
	// std::cout << "end idx is " << this->_msgLength << std::endl;
	imgBuf = file.rdbuf();
	for ( size_t i = 0; i < lengthFile; i++ ) {
		response[this->_msgLength + i] = imgBuf->sbumpc();
		// std::cout << i << ":" << static_cast<int>(response[this->_msgLength + i]) << " ";
	}
	this->_msgLength += lengthFile;
	file.close();
	return (response);
	
}

/**
 * @brief 
 * 
 * @param sb 
 * @return uint8_t* 
 */
uint8_t *	Response::createResponse()
{
	std::vector<Location>	locations;
	Location				a;
	Location				b;
	Location				c;

	a.setLocationMatch("/");
	a.setLocationModifier("=");
	a.addDirective("index", "index.html");
	b.setLocationMatch("/");
	b.setLocationModifier("");
	b.addDirective("root", "data/www");
	b.addErrorPage(404, "/404.html");
	c.setLocationMatch("/images");
	c.setLocationModifier("");
	c.addDirective("root", "data/images");
	c.addErrorPage(404, "/404.html");
	locations.push_back(a);
	locations.push_back(b);
	locations.push_back(c);
	
	if (this->_req.getMethod() == "GET")
	{
		// CONTINUE HERE 
	
		// compare target (only URI part, no arguments!) with all locations server->getLocations
			// 1. check the ones with "=" sign for exact matches
			// 2. if no exact matches found, evaluate non-exact prefixes 
			// map? location: root? -> not enough, there may be more items; maybe nested map?
			// locationsPrefixes 
		// choose the one with biggest overlap
		// then check the regex ones, if match, continue there, otherwise return to the longest overlap
		// add the target to the location's root
		
		if (this->_req.getTarget().find(".jpg") < std::string::npos)
			return (this->createResponseImg());
		else
			return (this->createResponseHtml());
	}
	return (NULL);
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
		// std::cout << "--> Len is " << len << std::endl;
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

Request &	Response::getRequest(void)
{
	return (this->_req);
}

int	Response::getErrorCode(void)
{
	return (this->_errorCode);
}