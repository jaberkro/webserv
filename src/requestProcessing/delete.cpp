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

bool allowedToDelete(std::string toRemove, std::vector<Location>::const_iterator const & location)
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

// DM: function deleteFile moved to the Response class (that made it easier to amend the status codes necessary for preparation of the response)

// DM: I suggest moving these two remaining functions into the file utils.cpp in the requestProcessing folder
