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
	std::string	line;
	Server		newServer;
	int			directive;

	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
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
			directive = hasInheritanceDirective(line);
			if (directive == -1)
			{
				std::cout << "Error: can't parse server block near [" << line << "]" << std::endl;
				exit(EXIT_FAILURE);
			}
			values = parseInheritanceDirective(directive, line, values);
		}
	}
	if (line != "}")
	{
		std::cout << "Error: server block not closed before end of file" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (newServer.getListens().size() == 0)
		newServer.addListen(std::make_pair("0.0.0.0", 80));
	// if (!values.errorPages.empty())
	// {
	// 	std::cout << "no error pages in server" << std::endl;
	// 	newServer.setErrorPages(values.errorPages);
	// }
	if (newServer.getLocations().size() == 0)
	{
		std::cout << "Error: can't parse server block without location block inside of it: \nserver {\n\tlocation <optional modifier> <match>{\n\n\t}\n}" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (newServer);
}
