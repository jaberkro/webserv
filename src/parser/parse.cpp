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
			return (0);
	}
	if (configFile.at(configFile.size() - 1 - extension.size()) == '.')
		return (0);
	return (1);
}

/**
 * @brief open configuration file and test if open failed or not
 * 
 * @param configFile the name of the file to open
 * @return std::fstream the file that is opened
 */
static std::fstream openFile(char *configFile)
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

/**
 * @brief 
 * 
 * 
 * @param servers variable to store the parsed information in
 * @param configFile title of the file to parse
 */
void parse(std::vector<Server> &servers, char *configFile)
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
		hasDirective(line) != -1) // location must have this error message? // and allow?
		{
			std::cerr << "Error: [" << line << "]: should be inside ";
			std::cerr << "http block: \nhttp {\n\n}" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
			notRecognizedError(line, "configuration file");
	}
	//what if 2 http under each other
	if (servers.size() == 0) //  beter checken, noserveerrpr in http
	{
		std::cerr << "Error: http block missing: \nhttp {\n\n}" << std::endl; // if you open a directory, you get this error as well. How to catch directories before even opening it?
		exit(EXIT_FAILURE);
	}
}
