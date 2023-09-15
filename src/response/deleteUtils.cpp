#include "Request.hpp"
#include "Response.hpp"
#include "Location.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sys/stat.h>

/**
 * @brief predefined directories and files that are not accessible for delete
 * 
 * @param toRemove the file that the user wants to remove
 * @return true it is forbidden to delete this, based on location
 * @return false it is not forbidden to delete this, based on location
 */
bool forbiddenToDeleteFileOrFolder(std::string toRemove) // JMA: should these ones be removed? data/www, data/images?
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

std::string createRemovePath(Request req, locIterator loc)
{
	std::string toRemove;
	size_t		queryStart = req.getQueryString().find_last_of("=") + 1;
	std::string	queryString = req.getQueryString().substr(queryStart);

	if (req.getHeaders()["User-Agent"].find("curl") == 0)
		toRemove = loc->getRoot() + req.getTarget();
	else
		toRemove = loc->getUploadDir() + "/" + queryString;
	return (toRemove);
}

/**
 * @brief execute the DELETE method on the target defined in the request.
 * Sets the _statusCode based on the result. OK or DELETED means that the
 * DELETE is executed succesfully.
 */
int	deleteFile(Request req, locIterator loc)
{
	std::string	toRemove;
	struct stat fileInfo;

	std::cout << "\nDELETE method requested" << std::endl;
	toRemove = createRemovePath(req, loc);
	// std::cout << "DELETE path: " << toRemove << std::endl;
	if (forbiddenToDeleteFileOrFolder(toRemove))
		return (FORBIDDEN);
	if (stat(toRemove.c_str(), &fileInfo) != 0)	// DM: this seems not to be working
		return (NOT_FOUND);
	if (fileInfo.st_mode & S_IFDIR || remove(toRemove.c_str()) != 0)
		return (BAD_REQUEST);
	std::cout << "DELETED!\n";
	if (req.getHeaders()["User-Agent"].find("curl") == 0)
		return (DELETED);
	return (OK);
}
