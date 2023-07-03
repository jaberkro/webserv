#include "Location.hpp"
#include "parse.hpp"
#include <iostream>
#include <fstream>
#include <utility>

static std::string findMatch(std::string &line)
{
	if (line.size() < 2 || findFirstWhitespace(line) == line.size())
	{
		std::cout << "Error: invalid location block near match [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (line.substr(0, findFirstWhitespace(line)));
}

static std::string findModifier(std::string &line)
{
	if (line.size() < 2)
	{
		std::cout << "Error: invalid location block near [location " << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.find("= ") == 0)
		return ("=");
	else if (line.find("~ ") == 0)
		return ("~");
	else if (line.find("~* ") == 0)
		return ("~*");
	else if (line.find("^~ ") == 0)
		return ("^~");
	return ("(none)");
}

Location parseLocation(std::fstream &file, std::string line, t_values values)
{
	Location location;
	int		directive;

	line = protectedSubstr(line, 8);
	line = ltrim(line);
	line.pop_back();

	location.setModifier(findModifier(line));
	if (location.getModifier() != "(none)")
	{
		line = protectedSubstr(line, findFirstWhitespace(line));
		line = ltrim(line);
	}

	location.setMatch(findMatch(line));
	if (findFirstWhitespace(line) != line.size())
	{
		line = protectedSubstr(line, findFirstWhitespace(line));
		line = ltrim(line);
		if (line != "")
		{
			std::cout << "Error: invalid location block near match: too much arguments: [" << line << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	while (getValidLine(file, line))
	{
		if (line == "")
			std::cout << "empty line in location block" << std::endl;
		else if (line == "}")
			break ;
		else if (line.find("location") == 0 && line.back() == '{')
		{
			std::cout << "Error: can't parse location block inside location block; not implemented" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			directive = hasInheritanceDirective(line);
			if (directive == -1)
			{
				std::cout << "Error: can't parse location block near [" << line << "]" << std::endl;
				exit(EXIT_FAILURE);
			}
			values = parseInheritanceDirective(directive, line, values);
		}
	}
	location.setRoot(values.root);
	location.setIndexes(values.indexes);
	location.setAutoindex(values.autoindex);
	location.setMaxBodySize(values.maxBodySize);
	return (location);
}

