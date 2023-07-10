#include "parse.hpp"
#include "Server.hpp"
#include <iostream>
#include <fstream>
#include <string>

/**
 * @brief open configuration file and test if open failed or not
 * 
 * @param configFile the name of the file to open
 * @return std::fstream the file that is opened
 */
std::fstream openFile(char *configFile)
{
	std::fstream file;

	file.open(configFile, std::fstream::in);
	if (file.is_open())
		return (file);
	std::cout << "Error opening configuration file" << std::endl;
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
	t_values		values;

	file = openFile(configFile);
	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
		else if (line == "http {")
		{
			parseHTTP(servers, file, values);
		}
		else if (line == "server {" || line.find("location") == 0 || hasInheritanceDirective(line) != -1)
		{
			std::cout << "Error: http block missing: \nhttp {\n\n}" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			std::cout << "Error: can't parse [" << line << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (servers.size() == 0)
	{
		std::cout << "Error: http block missing: \nhttp {\n\n}" << std::endl; // if you open a directory, you get this error as well. How to catch directories before even opening it?
		exit(EXIT_FAILURE);
	}
}
