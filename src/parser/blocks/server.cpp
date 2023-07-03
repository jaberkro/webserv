#include "Server.hpp"
#include "parse.hpp"
#include <iostream>

/**
 * @brief parse a server block
 * 
 * @param config the variable to store the parsed information in
 * @param file the file to parse the information from
 */
Server  parseServer(std::fstream &file, t_values values)
{
	std::string					line;
	Server newServer;
	int	directive;

	while (getValidLine(file, line))
	{
		if (line == "")
			std::cout << "empty line in server block" << std::endl;
		else if (line == "}")
			break ;
		else if (line.find("location") == 0 && line.back() == '{')
			newServer.addLocation(parseLocation(file, line, values));
		else if (line.find("listen") == 0)
			newServer.addListen(parseListen(line));
		else if (line.find("server_name") == 0)
			parseServerNames(newServer, line);
		else
		{
			directive = hasDirective(line);
			if (directive == -1)
			{
				std::cout << "Error: can't parse server block near [" << line << "]" << std::endl;
				exit(EXIT_FAILURE);
			}
			values = parseDirective(directive, line, values);
		}
	}
	if (newServer.getListens().size() == 0)
		newServer.addListen(std::make_pair("0.0.0.0", 80));
	return (newServer);
}
