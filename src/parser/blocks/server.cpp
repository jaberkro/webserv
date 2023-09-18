#include "Server.hpp"
#include "parse.hpp"
#include <iostream>

static void storeValuesInServer(Server &newServer, t_values values) // only error pages and missing listen
{
	if (newServer.getLocations().size() == 0)
		noLocationError();
	if (newServer.getListens().size() == 0)
		newServer.addListen(std::make_pair("0.0.0.0", 80));
	if (!values.errorPages.empty())
		newServer.setErrorPages(values.errorPages);
}

static void checkNotImplementedServer(std::string line)
{
	if (line.find("http") == 0)
		notImplementedError(line, "server", "begin of configuration file");
	else if (line.find("allow") == 0)
		notImplementedError(line, "server", "location block");
	else if (line.find("deny") == 0)
		notImplementedError(line, "server", "location block");
	else if (line.find("CGI") == 0)
		notImplementedError(line, "server", "location block");
}

static int parsedImplementedServerOnly(Server &newServer, std::string line, \
	std::fstream &file, t_values values)
{
	if (line.find("location") == 0 && line.back() == '{')
		newServer.addLocation(parseLocation(file, line, values));
	else if (line.find("listen") == 0)
		newServer.addListen(parseListen(line));
	else if (line.find("server_name") == 0)
		parseServerNames(newServer, line);
	else
		return (0);
	return (1);
}

/**
 * @brief parse a server block
 * 
 * @param config the variable to store the parsed information in
 * @param file the file to parse the information from
 */
Server parseServer(std::fstream &file, t_values values)
{
	std::string	line;
	Server		newServer;

	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
		else if (line == "}")
			break ;
		else if (!parsedImplementedServerOnly(newServer, line, file, values))
		{
			checkNotImplementedServer(line);
			if (hasDirective(line) == -1)
				notRecognizedError(line, "server block");
			values = parseDirective(hasDirective(line), line, values);
		}
	}
	if (line != "}")
		notClosedError("server");
	storeValuesInServer(newServer, values);
	return (newServer);
}
