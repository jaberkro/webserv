#include "Response.hpp"
#include <sys/stat.h>

bool forbiddenToDeleteFileOrFolder(std::string toRemove)
{
	if (toRemove.find("../") != std::string::npos || \
		toRemove.find("./") == 0 || \
		toRemove.find(".github/") == 0 || \
		toRemove.find("cgi-bin/") == 0 || \
		toRemove.find("config/") == 0 || \
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

int	deleteFile(Request req, locIterator loc)
{
	std::string	toRemove;
	struct stat fileInfo;

	toRemove = createRemovePath(req, loc);
	if (forbiddenToDeleteFileOrFolder(toRemove))
		return (FORBIDDEN);
	if (stat(toRemove.c_str(), &fileInfo) != 0)
		return (NOT_FOUND);
	if (fileInfo.st_mode & S_IFDIR || remove(toRemove.c_str()) != 0)
		return (BAD_REQUEST);
	if (req.getHeaders()["User-Agent"].find("curl") == 0)
		return (DELETED);
	return (OK);
}
