#include "parse.hpp"
#include <iostream>
#include <fstream>

void	storeValuesInLocation(t_values values, Location &location)
{
	if (values.root != "")
		location.setRoot(values.root);
	else
		location.setRoot("data/www");
	location.setIndexes(values.indexes);
	location.setAutoindex(values.autoindex);
	location.setMaxBodySize(values.maxBodySize);
	location.setErrorPages(values.errorPages);
	location.setAllowed(values.allowed);
	location.setDenied(values.denied);
	location.setReturn(values.returnCode, values.returnText);
	location.setCgiScriptName(values.CGI);
	if (values.uploadDir != "")
		location.setUploadDir(values.uploadDir);
	else
		location.setUploadDir("uploads");
}

static void checkNotImplementedLocation(std::string line)
{
	if (line.find("location") == 0)
		notImplementedError(line, "location", "server block");
	else if (line == "http {")
		notImplementedError(line, "location", "top of configuration file");
	else if (line == "server {")
		notImplementedError(line, "location", "http block");
	else if (line.find("listen") == 0)
		notImplementedError(line, "location", "server block");
	else if (line.find("server_name") == 0)
		notImplementedError(line, "location", "server block");
	else if (line.find("upload_dir") == 0)
		notImplementedError(line, "location", "server or http block");
}

void parseModifierAndMatch(Location &location, std::string &line)
{
	line = protectedSubstr(line, 8, line.size() - 9);
	line = ltrim(line);
	line = rtrim(line);
	checkEmptyString(line, "location", "not enough arguments");
	if (line.at(0) == '=' && firstWhitespace(line) == 1)
	{
		location.setModifier("=");
		line = protectedSubstr(line, firstWhitespace(line));
		line = ltrim(line);
	}
	else
		location.setModifier("(none)");
	checkEmptyString(line, "location", "not enough arguments");
	location.setMatch(protectedSubstr(line, 0, firstWhitespace(line)));
}

Location parseLocation(std::fstream &file, std::string line, t_values values)
{
	Location	location;

	parseModifierAndMatch(location, line);
	checkOneArgumentOnly(line, "location match");
	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
		else if (line == "}")
			break ;
		else
		{
			checkNotImplementedLocation(line);
			if (hasDirective(line) == -1 && hasLocDirective(line) == -1)
				notRecognizedError(line, "location block");
			else if (hasDirective(line) == -1)
				values = parseLocDirective(hasLocDirective(line), line, values);
			else
				values = parseDirective(hasDirective(line), line, values);
		}
	}
	if (line != "}")
		notClosedError("location");
	storeValuesInLocation(values, location);
	return (location);
}
