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

	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
		else if (line == "}")
			break ;
		else if (line == "server {")
			servers.push_back((parseServer(file, values)));
		else if (line.find("location") == 0)
		{
			std::cout << "Error: [" << line << "] should be inside a server block:\nserver {\n\tlocation <modifier> <match> {\n\n\t}\n}" << std::endl;
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
			values = parseInheritanceDirective(directive, line, values);
		}
	}
	if (servers.size() == 0)
	{
		std::cout << "Error: can't parse http block without server block inside of it: \nhttp {\n\tserver {\n\n\t}\n}" << std::endl;
		exit(EXIT_FAILURE);
	}
	return ;
}