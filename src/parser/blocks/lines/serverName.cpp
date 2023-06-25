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
	line = line.substr(11);
	line = ltrim(line);
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		server.addServerName(line.substr(0, findFirstWhitespace(line)));
		line = line.substr(findFirstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
		server.addServerName(line);
}