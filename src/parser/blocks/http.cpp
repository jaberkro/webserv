#include "Config.hpp"
#include "parse.hpp"
#include <fstream>
#include <iostream>
#include <string>

/**
 * @brief parse HTTP block
 * 
 * @param config the variable to store the parsed information in
 * @param file the file to parse the information from
 */
void parseHTTP(Config &config, std::fstream &file, t_values values)
{
	std::string line;
	int			directive;

	while (getValidLine(file, line))
	{
		if (line == "")
			std::cout << "empty line in http block" << std::endl;
		else if (line == "}")
			break ;
		else if (line == "server {")
			config.addServer(parseServer(file, values));
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
	if (config.getServers().size() == 0)
	{
		std::cout << "Error: can't parse http block without server block" << std::endl;
		exit(EXIT_FAILURE);
	}
	return ;
}