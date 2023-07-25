#include "Server.hpp"
#include "parse.hpp"
#include <string>

/**
 * @brief parse a server_name command
 * 
 * @param server the server where found serverNames should be added to
 * @param line the std::string to parse the serverNames from
 */
void parseServerNames(Server &server, std::string &line)
{
	std::string reason = "needs at least one argument: server_name <name>";

	line = protectedSubstr(line, 11);
	line = ltrim(line);
	checkEmptyString(line, "server_name", reason);
	while (firstWhitespace(line) != line.size() && firstWhitespace(line) != 0)
	{
		server.addServerName(protectedSubstr(line, 0, firstWhitespace(line)));
		line = protectedSubstr(line, firstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
		server.addServerName(line);
}
