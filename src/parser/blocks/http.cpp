#include "Server.hpp"
#include "parse.hpp"
#include <fstream>
#include <iostream>
#include <string>

static void checkNotImplementedHTTP(std::string line)
{
	if (line.find("location") == 0)
		notImplementedError(line, "http", "server block");
	else if (line.find("listen") == 0)
		notImplementedError(line, "http", "server block");
	else if (line.find("server_name") == 0)
		notImplementedError(line, "http", "server block");
	else if (line.find("error_page") == 0)
		notImplementedError(line, "http", "server or location block");
	else if (line.find("return") == 0)
		notImplementedError(line, "http", "server or location block");
	else if (line.find("allow") == 0)
		notImplementedError(line, "http", "location block");
	else if (line.find("deny") == 0)
		notImplementedError(line, "http", "location block");
	else if (line.find("CGI") == 0)
		notImplementedError(line, "http", "location block");
}

static int parsedImplementedHTTPOnly(std::vector<Server> &servers, \
	std::string line, std::fstream &file, t_values values)
{
	if (line == "server {")
			servers.push_back(parseServer(file, values));
	else
		return (0);
	return (1);
}

/**
 * @brief parse HTTP block
 * 
 * @param servers the variable to store the parsed information in
 * @param file the file to parse the information from
 */
void parseHTTP(std::vector<Server> &servers, std::fstream &file)
{
	std::string line;
	t_values	values;

	values = fillDefaultErrorPages(values);
	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
		else if (line == "}")
			break ;
		else if (!parsedImplementedHTTPOnly(servers, line, file, values))
		{
			checkNotImplementedHTTP(line);
			if (hasDirective(line) == -1)
				notRecognizedError(line, "http block");
			values = parseDirective(hasDirective(line), line, values);
		}
	}
	if (line != "}")
		notClosedError("HTTP");
	if (servers.size() == 0)
		noServerError();
}
