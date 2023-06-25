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
	if (line.size() < 3)
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

Location parseLocation(std::fstream &file, std::string line)
{
	Location location;

	line = line.substr(8);
	line = ltrim(line);
	line.pop_back();
	
	location.setModifier(findModifier(line));
	if (location.getModifier() != "(none)")
	{
		line = line.substr(findFirstWhitespace(line));
		line = ltrim(line);
	}
	
	location.setMatch(findMatch(line));
	if (findFirstWhitespace(line) != line.size())
	{
		line = line.substr(findFirstWhitespace(line));
		line = ltrim(line);
		if (line != "")
		{
			std::cout << "Error: invalid location block near match: too much arguments: [" << line << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	// std::cout << "[" << location.getMatch() << "]" << std::endl;
	while (getValidLine(file, line))
	{
		if (line == "}")
			break ;
		else if (line.find("location") == 0) // add more checks here
		{
			std::cout << "Error: can't parse location block inside location block; not implemented" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			std::cout << "Error: can't parse [" << line << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return (location);
}

