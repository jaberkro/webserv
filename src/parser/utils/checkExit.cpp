#include "parse.hpp"
#include <string>
#include <iostream>

void checkOneArgumentOnly(std::string line, std::string directive)
{
	if (firstWhitespace(line) != line.size())
	{
		std::cerr << "Error: can't parse " << directive;
		std::cerr << ": too many arguments: [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkEmptyString(std::string line, std::string directive, std::string why)
{
	if (line == "")
	{
		std::cerr << "Error: can't parse " << directive << " directive: ";
		std::cerr << why << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkNoEndingSlash(std::string line, std::string directive)
{
	if (line.find_last_of("/") == line.size() - 1)
	{
		std::cerr << "Error: can't parse " << directive << ": ";
		std::cerr << "path should not end with '/': ";
		std::cerr << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkStartingSlash(std::string line, std::string directive)
{
	if (line.find("/") != 0)
	{
		std::cerr << "Error: can't parse " << directive << ": ";
		std::cerr << "path should start with '/': ";
		std::cerr << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkHasDot(std::string line, std::string directive)
{
	if (line.find(".") == std::string::npos || \
		line.find(".") == line.size() - 1)
	{
		std::cerr << "Error: " << directive << " argument needs to have: ";
		std::cerr << "a name and extension seperated by '.': ";
		std::cerr << directive << " <fileName>.<extension>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}
