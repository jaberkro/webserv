#include "Response.hpp"

#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include "server.hpp"
#include <cstdio>

Response::Response(Request req) : _req (req) {}

Response::~Response(void) {}

void	Response::createResponse(uint8_t *response)
{
	std::string		pathToFile;
	std::ifstream	file;
	std::string		contentType;
	uint8_t			fileBuf[MAXLINE + 1];

	std::memset(response, 0, MAXLINE);
	if (this->_req.getMethod() == "GET")
	{
		pathToFile = this->_req.getTarget() == "/" ? "data/www/index.html" : \
		this->_req.getTarget().find(".jpg") ? "data" + this->_req.getTarget() : "data/www" + this->_req.getTarget();
		contentType = pathToFile.find("images") ? "image/jpg" : "text/html";
		std::cout << "Path to file: " << pathToFile << std::endl;
		try
		{
			file.open(pathToFile);
			file.read((char *)fileBuf, MAXLINE);

			snprintf((char *)response, sizeof(response), \
			"%s 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s", \
			this->_req.getProtocolVersion().c_str(), contentType.c_str(), std::strlen((char *)fileBuf), (char *)fileBuf);
			file.close();
		}
		catch(const std::ios_base::failure& f)
		{
			std::cerr << "Caught an exception: " << f.what() << std::endl;
			snprintf((char *)response, sizeof(response), "%s 404 Not Found", this->_req.getProtocolVersion().c_str());
		}
		// return (response);
	}
	// return (response);
	// std::cout << "[create response:] " << &this->_req << std::endl;
	// if (this->_req.getHeaders().find("Sec-Fetch-Dest") != this->_req.getHeaders().end())
	// 	std::cout << this->_req.getHeaders().find("Sec-Fetch-Dest")->second << std::endl;
	// // else image ...
}