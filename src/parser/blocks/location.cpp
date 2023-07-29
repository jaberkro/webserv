#include "Location.hpp"
#include "parse.hpp"
#include <iostream>
#include <fstream>
#include <utility>

void	storeValuesInLocation(t_values values, Location &location)
{
	location.setRoot(values.root);
	location.setIndexes(values.indexes);
	location.setAutoindex(values.autoindex);
	location.setMaxBodySize(values.maxBodySize);
	location.setErrorPages(values.errorPages);
	location.setAllowed(values.allowed);
	location.setDenied(values.denied);
	location.setReturn(values.returnCode, values.returnText);
	location.setUploadDir(values.uploadDir);
}

static void checkNotImplementedLocation(std::string line)
{
	if (line.find("location") == 0)
		notImplementedError(line, "location", "server block");
	else if (line.find("http") == 0)
		notImplementedError(line, "location", "begin of configuration file");
	else if (line.find("listen") == 0)
		notImplementedError(line, "location", "server block");
	else if (line.find("server_name") == 0)
		notImplementedError(line, "location", "server block");
}

void parseModifierAndMatch(Location &location, std::string &line)
{
	line = protectedSubstr(line, 8);
	line = ltrim(line);
	line.pop_back();
	line = rtrim(line);
	checkEmptyString(line, "location", "not enough arguments");
	if (line.find("= ") == 0)
		location.setModifier("=");
	else
		location.setModifier("(none)");
	if (location.getModifier() != "(none)")
	{
		line = protectedSubstr(line, firstWhitespace(line));
		line = ltrim(line);
	}
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
				notRecognizedError(line, "location");
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
