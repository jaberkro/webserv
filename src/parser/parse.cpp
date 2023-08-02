#include "parse.hpp"
#include "Server.hpp"
#include <iostream>
#include <fstream>
#include <string>

static bool validExtension(std::string configFile, std::string extension)
{
	for(size_t i = 0; i < extension.size(); i++)
	{
		if (configFile.at(configFile.size() - 1 - i) != \
			extension.at(extension.size() - 1 - i))
			return (0);
	}
	// check if not ..conf
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
	std::fstream file;

	if (!validExtension(configFile, ".conf"))
	{
		std::cout << "Error: file extension should be '.conf'" << std::endl;
		exit(EXIT_FAILURE);
	}
	file.open(configFile, std::fstream::in);
	if (file.is_open()) // if not open, then error
		return (file); 
	std::cout << "Error: not possible to open configuration file" << std::endl; //std::cerr
	exit(EXIT_FAILURE);
}

/**
 * @brief parse configuration file
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
		else if (line == "http {")
			parseHTTP(servers, file);
		else if (line == "server {" || line.find("location") == 0 || \
			hasDirective(line) != -1) // location msut have this error message? // and allow?
		{
			std::cout << "Error: [" << line << "]: should be inside ";
			std::cout << "http block: \nhttp {\n\n}" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
			notRecognizedError(line, "");
	}
	//what if 2 http under each other
	if (servers.size() == 0) //  beter checken, noserveerrpr in http
	{
		std::cout << "Error: http block missing: \nhttp {\n\n}" << std::endl; // if you open a directory, you get this error as well. How to catch directories before even opening it?
		exit(EXIT_FAILURE);
	}
}
