#include "parse.hpp"
#include "Server.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>

static bool validExtension(std::string configFile, std::string extension)
{
	if (configFile.size() <= extension.size())
		return (0);
	for(size_t i = 0; i < extension.size(); i++)
	{
		if (configFile.at(configFile.size() - 1 - i) != \
		extension.at(extension.size() - 1 - i))
		{
			return (0);
		}
	}
	if (configFile.at(configFile.size() - 1 - extension.size()) == '.')
		return (0);
	return (1);
}

static std::fstream openFile(const char *configFile)
{
	std::fstream	file;
	struct stat		fileInfo;

	if (!validExtension(configFile, ".conf"))
	{
		std::cerr << "Error: file extension should be '.conf'" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (stat(configFile, &fileInfo) != 0)
	{
		std::cerr << "Error: file does not exist" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (fileInfo.st_mode & S_IFDIR)
	{
		std::cerr << "Error: directory not allowed as input" << std::endl;
		exit(EXIT_FAILURE);
	}
	file.open(configFile, std::fstream::in);
	if (!file.is_open())
	{
		std::cerr << "Error: opening configuration file failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (file);
}

void parse(std::vector<Server> &servers, const char *configFile)
{
	std::fstream	file;
	std::string		line;

	file = openFile(configFile);
	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
		else if (line == "http {" && servers.size() == 0)
			parseHTTP(servers, file);
		else if (line == "http {")
		{
			std::cerr << "Error: there can only be one http block" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (line == "server {" || line.find("location") == 0 || \
		hasDirective(line) != -1)
		{
			notImplementedError(line, "outer layer of configuration file", \
				"http block: \nhttp {\n\n}");
		}
		else
			notRecognizedError(line, "configuration file");
	}
	checkMissingHTTP(servers);
}
