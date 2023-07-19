#include "Server.hpp"
#include "parse.hpp"
#include <fstream>
#include <iostream>
#include <string>

/**
 * @brief parse HTTP block
 * 
 * @param servers the variable to store the parsed information in
 * @param file the file to parse the information from
 */
void parseHTTP(std::vector<Server> &servers, std::fstream &file, t_values values)
{
	std::string line;
	int			directive;
	Server		newestServer;
	
	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
		else if (line == "}")
			break ;
		else if (line == "server {")
		{
			newestServer = parseServer(file, values);
			servers.push_back(newestServer);
		}
		else if (line.find("error_page") == 0)
		{
			std::cout << "Error: [" << line << "]: not implemented in http block: can only be inside a server or location block" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (line.find("location") == 0)
		{
			std::cout << "Error: [" << line << "] should be inside a server block:\nserver {\n\tlocation <optional modifier> <match> {\n\n\t}\n}" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			directive = hasInheritanceDirective(line);
			if (directive == -1)
			{
				std::cout << "Error: can't parse http block near [" << line << "]" << std::endl;
				exit(EXIT_FAILURE);
			}
			else if (directive == 5)
			{
				std::cout << "Error: can't parse return directive in http block: [" << line << "]" << std::endl;
				exit(EXIT_FAILURE);
			}
			values = parseInheritanceDirective(directive, line, values);
		}
	}
	if (line != "}")
	{
		std::cout << "Error: http block not closed before end of file" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (servers.size() == 0)
	{
		std::cout << "Error: can't parse http block without server block inside of it: \nhttp {\n\tserver {\n\n\t}\n}" << std::endl;
		exit(EXIT_FAILURE);
	}
	return ;
}