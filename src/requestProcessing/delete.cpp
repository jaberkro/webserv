#include "Request.hpp"
#include "Location.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sys/stat.h>

static bool forbiddenFileOrFolder(std::string toRemove)
{
	if (toRemove.find("../") != std::string::npos || \
		toRemove.find("./") == 0 || \
		toRemove.find(".github/") == 0 || \
		toRemove.find("cgi-bin/") == 0 || \
		toRemove.find("config/") == 0 || \
		toRemove.find("data/www/") == 0 || \
		toRemove.find("data/images/") == 0 || \
		toRemove.find("error_tests/") == 0 || \
		toRemove.find("src/") == 0 || \
		toRemove.find("include/") == 0 || \
		toRemove == "Makefile" || \
		toRemove == "README.md" || \
		toRemove == "webserv")
	{
		std::cout << "location not allowed: " << toRemove << std::endl;
		return (1);
	}
	std::cout << "location allowed in theory" << std::endl;
	return (0);
}

static bool allowedToDelete(std::string toRemove, std::vector<Location>::const_iterator const & location)
{
	int	allowCount = 0;

	if (forbiddenFileOrFolder(toRemove))
		return (0);
	for (size_t i = 0; i < location->getAllowed().size(); i++)
	{
		if (location->getAllow(i) == "DELETE")
			allowCount++;
	}
	for (size_t i = 0; i < location->getDenied().size(); i++)
	{
		if (location->getDeny(i) == "DELETE" || (allowCount == 0 && location->getDeny(i) == "all"))
			return (0);
	}
	return (1);
}

std::string	deleteFile(Request request, std::vector<Location>::const_iterator const & location)
{
	std::string	toRemove;
	struct stat fileInfo;

	std::cout << "\nATTEMPT TO DELETE RIGHT NOW!!!" << std::endl;
	if (request.getMethod() == "GET")
		toRemove = location->getUploadDir() + "/" + request.getQueryString().substr(request.getQueryString().find_last_of("=") + 1);
	else
		toRemove = location->getRoot() + request.getTarget();
	std::cout << "DELETE path: " << toRemove << std::endl;
	if (!allowedToDelete(toRemove, location))
		return ("403 Not allowed\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\nNot allowed\n");
	if (stat(toRemove.c_str(), &fileInfo) != 0)
		return ("404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 10\r\n\r\nNot found\n");
	if (fileInfo.st_mode & S_IFDIR || remove(toRemove.c_str()) != 0)
		return ("400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\nBad request\n");
	std::cout << "DELETE SUCCESSFUL!\n" << std::endl;
	return ("204 Deleted\r\n");
}
