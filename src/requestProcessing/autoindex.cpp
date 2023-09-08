#include <string>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

static std::string createAutoindexEnd(void)
{
	std::fstream	endFile;
	std::string		line;
	std::string		endMessage;

	endFile.open("data/www/.autoindexEnd.html", std::fstream::in);
	if (!endFile.is_open())
	{
		std::cout << "OPENING ENDFILE FAILED!" << std::endl;
		return ("");
	}
	else
	{
		std::cout << "OPENING ENDFILE WORKED!" << std::endl;
		while (std::getline(endFile, line))
		{
			endMessage.append(line);
			endMessage.append("\n");
		}
		endFile.close();
	}
	return (endMessage);
}

static std::string createAutoindexMiddle(std::string oldPath)
{
	std::string		fileRefs = "";
	DIR				*directory;
	struct dirent	*entry;
	std::string		newPath;
	std::string		newPathURI;
	struct stat		pathInfo;

	directory = opendir(oldPath.c_str());
	if (directory == NULL)
	{
		std::cerr << "OPENDIR FAILED" << std::endl;
		return (fileRefs);
	}
	entry = readdir(directory);
	while (entry != NULL)
	{
		if (entry->d_name[0] != '.')
		{
			newPath = oldPath + "/" + entry->d_name;
			if (stat(newPath.c_str(), &pathInfo) != 0)
			{
				std::cerr << "STAT FAILED" << std::endl;
				return ("");
			}
			else if (S_ISDIR(pathInfo.st_mode))
			{
				fileRefs.append(createAutoindexMiddle(newPath));
			}
			else
			{
				newPath = newPath.substr(5);
				if (newPath.find("www/") == 0)
					newPathURI = newPath.substr(4);
				// else if (newPath.find("uploads/") == 0)
				// 	newPathURI = newPath.substr(8);
				else
					newPathURI = newPath;
				fileRefs.append("\t\t\t<a href=" + newPathURI + ">" + newPath + "</a><br>\n");
			}

		}
		entry = readdir(directory);
	}
	closedir(directory);
	return (fileRefs);
}

static std::string createAutoindexStart(void)
{
	std::fstream	startFile;
	std::string		line;
	std::string		startMessage;

	startFile.open("data/www/.autoindexStart.html", std::fstream::in);
	if (!startFile.is_open())
	{
		std::cout << "OPENING STARTFILE FAILED!" << std::endl;
		return ("");
	}
	else
	{
		std::cout << "OPENING STARTFILE WORKED!" << std::endl;
		while (std::getline(startFile, line))
		{
			startMessage.append(line);
			startMessage.append("\n");
		}
		startFile.close();
	}
	return (startMessage);
}

std::string createAutoindex(void)
{
	std::string		message;
	std::fstream	endFile;

	message.append(createAutoindexStart());
	message.append(createAutoindexMiddle("data"));
	message.append(createAutoindexEnd());

	std::cout << "AUTOINDEX CREATION:\n\n" << message << std::endl;
	return (message);
}
