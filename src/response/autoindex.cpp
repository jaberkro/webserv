#include "Response.hpp"
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

static std::string createAutoindexMid(std::string oldPath, Response &response);

static std::string createAutoindexEnd(Response &response)
{
	std::fstream	endFile;
	std::string		line;
	std::string		endMessage;

	endFile.open("data/www/.autoindexEnd.html", std::fstream::in);
	if (!endFile.is_open())
	{
		response.setStatusCode(NOT_FOUND);
		return ("");
	}
	else
	{
		while (std::getline(endFile, line))
		{
			endMessage.append(line);
			endMessage.append("\n");
		}
		endFile.close();
	}
	return (endMessage);
}

static void	createNewPath(std::string oldPath, struct dirent entry, \
	struct stat pathInfo, Response &response, std::string &fileRefs)
{
	std::string		newPath;
	std::string		newPathURI;

	newPath = oldPath + "/" + entry.d_name;
	if (stat(newPath.c_str(), &pathInfo) != 0)
	{
		response.setStatusCode(NOT_FOUND);
		return ;
	}
	else if (S_ISDIR(pathInfo.st_mode))
		fileRefs.append(createAutoindexMid(newPath, response));
	else
	{
		newPath = newPath.substr(5);
		if (newPath.find("www/") == 0)
			newPathURI = newPath.substr(4);
		else
			newPathURI = newPath;
		fileRefs.append("\t\t\t<a href=" + newPathURI + ">" + newPath + \
			"</a><br>\n");
	}
}

static std::string createAutoindexMid(std::string oldPath, Response &response)
{
	std::string		fileRefs = "";
	DIR				*directory;
	struct dirent	*entry;
	struct stat		pathInfo;

	directory = opendir(oldPath.c_str());
	if (directory == NULL)
	{
		response.setStatusCode(NOT_FOUND);
		return (fileRefs);
	}
	entry = readdir(directory);
	while (entry != NULL)
	{
		if (entry->d_name[0] != '.')
			createNewPath(oldPath, *entry, pathInfo, response, fileRefs);
		if (response.getStatusCode() == NOT_FOUND)
			return "";
		entry = readdir(directory);
	}
	closedir(directory);
	return (fileRefs);
}

static std::string createAutoindexStart(Response &response)
{
	std::fstream	startFile;
	std::string		line;
	std::string		startMessage;

	startFile.open("data/www/.autoindexStart.html", std::fstream::in);
	if (!startFile.is_open())
	{
		response.setStatusCode(NOT_FOUND);
		return ("");
	}
	else
	{
		while (std::getline(startFile, line))
		{
			startMessage.append(line);
			startMessage.append("\n");
		}
		startFile.close();
	}
	return (startMessage);
}

std::string createAutoindex(Response &response)
{
	std::string		message;
	std::fstream	endFile;

	message.append(createAutoindexStart(response));
	message.append(createAutoindexMid("data", response));
	message.append(createAutoindexEnd(response));
	return (message);
}
