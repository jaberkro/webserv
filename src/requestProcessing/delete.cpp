#include "Request.hpp"
#include "Location.hpp"
#include <string>
#include <vector>
#include <iostream>

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
		{
			std::cout << "DELETE is not part of the allowed methods in this location" << std::endl;
			return (0);
		}
	}
	return (1);
}

bool	deleteFile(Request request, std::vector<Location>::const_iterator const & location)
{
	std::string	toRemove;
	std::cout << "\nATTEMPT TO DELETE RIGHT NOW!!!\n" << std::endl;
	if (request.getMethod() == "GET")
		toRemove = location->getUploadDir() + "/" + request.getQueryString().substr(request.getQueryString().find_last_of("=") + 1);
	else
		toRemove = "data" + request.getTarget();
	std::cout << "DELETE path: " << toRemove << std::endl;
	if (!allowedToDelete(toRemove, location))
	{
		std::cout << "DELETE FAILED! NOT ALLOWED!!\n" << std::endl; // here for debug purposes, remove in the end
		return (0);
	}
	if (remove(toRemove.c_str()) != 0)
	{
		std::cout << "DELETE FAILED!!!\n" << std::endl; // here for debug purposes, remove in the end
		return (0);
	}
	std::cout << "DELETE SUCCESSFUL!!!\n" << std::endl; // here for debug purposes, remove in the end
	return (1);
}
