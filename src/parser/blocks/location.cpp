#include "Location.hpp"
#include "parse.hpp"
#include <iostream>
#include <fstream>
#include <utility>

static std::string findMatch(std::string &line)
{
	if (line.size() == 0)
	{
		std::cout << "Error: invalid location block: not enough arguments" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (protectedSubstr(line, 0, findFirstWhitespace(line)));
}

static std::string findModifier(std::string &line)
{
	if (line.size() == 0)
	{
		std::cout << "Error: invalid location block: not enough arguments" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.find("= ") == 0)
		return ("=");
	else if (line.find("~ ") == 0)
	{
		std::cout << "Error: invalid location block modifier: ~: regex not implemented" << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (line.find("~* ") == 0)
	{
		std::cout << "Error: invalid location block modifier: ~*: regex not implemented" << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (line.find("^~ ") == 0)
	{
		std::cout << "Error: invalid location block modifier: ^~: regex not implemented" << std::endl;
		exit(EXIT_FAILURE);
	}
	return ("(none)");
}

Location parseLocation(std::fstream &file, std::string line, t_values values)
{
	Location	location;
	int			directive;

	line = protectedSubstr(line, 8);
	line = ltrim(line);
	line.pop_back();
	line = rtrim(line);

	location.setModifier(findModifier(line));
	if (location.getModifier() != "(none)")
	{
		line = protectedSubstr(line, findFirstWhitespace(line));
		line = ltrim(line);
	}

	location.setMatch(findMatch(line));
	if (findFirstWhitespace(line) != line.size())
	{
		std::cout << "Error: invalid location block: too many matches: [" << line << "]" << std::endl; //remove regex as options?
		exit(EXIT_FAILURE);
	}
	while (getValidLine(file, line))
	{
		if (line == "")
			continue ;
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
				directive = hasLocationDirective(line);
				if (directive == -1)
				{
					std::cout << "Error: can't parse location block near [" << line << "]" << std::endl;
					exit(EXIT_FAILURE);
				}
				values = parseLocationDirective(directive, line, values);
			}
			else
				values = parseInheritanceDirective(directive, line, values);
		}
	}
	location.setRoot(values.root);
	location.setIndexes(values.indexes);
	location.setAutoindex(values.autoindex);
	location.setMaxBodySize(values.maxBodySize);
	location.setErrorPages(values.errorPages);
	location.setAllowed(values.allowed);
	location.setDenied(values.denied);
	return (location);
}

