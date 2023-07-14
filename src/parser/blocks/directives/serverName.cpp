#include "Server.hpp"
#include "parse.hpp"
#include <string>
# include <iostream>

/**
 * @brief parse a server_name command
 * 
 * @param server the server where found serverNames should be added to
 * @param line the std::string to parse the serverNames from
 */
void parseServerNames(Server &server, std::string &line)
{
	line = protectedSubstr(line, 11);
	line = ltrim(line);
	if (line == "")
	{
		std::cout << "Error: server_name needs at least one argument: server_name <name>;" << std::endl;
		exit(EXIT_FAILURE);
	}
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		server.addServerName(protectedSubstr(line, 0, findFirstWhitespace(line)));
		line = protectedSubstr(line, findFirstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
		server.addServerName(line);
}
