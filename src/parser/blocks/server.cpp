#include "Server.hpp"
#include "parse.hpp"
#include <iostream>

/**
 * @brief parse a server block
 * 
 * @param config the variable to store the parsed information in
 * @param file the file to parse the information from
 */
Server  parseServer(std::fstream &file)
{
	std::string					line;
	Server newServer;

	while (getValidLine(file, line))
	{
		if (line == "}")
			break ;
		else if (line.find("listen") == 0)
		{
			newServer.addListen(parseListen(line));
		}
		else if (line.find("server_name") == 0)
		{
			parseServerNames(newServer, line);
		}
		else if (line.find("location") == 0 && line.back() == '{')
		{
			newServer.addLocation(parseLocation(file, line));
		}
		else
		{
			std::cout << "Error: can't parse [" << line << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (newServer.getListens().size() == 0)
	{
		newServer.addListen(std::make_pair("0.0.0.0", 80));
	}
	return (newServer);
}