#include "parse.hpp"
#include <string>
#include <iostream>

void checkOneArgumentOnly(std::string line, std::string directive)
{
	if (firstWhitespace(line) != line.size())
	{
		std::cout << "Error: can't parse " << directive;
		std::cout << ": too many arguments: [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkEmptyString(std::string line, std::string directive, std::string why)
{
	if (line == "")
	{
		std::cout << "Error: can't parse " << directive << " directive: ";
		std::cout << why << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkNoEndingSlash(std::string line, std::string directive)
{
	if (line.find_last_of("/") == line.size() - 1)
	{
		std::cout << "Error: can't parse " << directive << ": ";
		std::cout << "path should not end with '/': ";
		std::cout << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkStartingSlash(std::string line, std::string directive)
{
	if (line.find("/") != 0)
	{
		std::cout << "Error: can't parse " << directive << ": ";
		std::cout << "path should start with '/': ";
		std::cout << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkHasDot(std::string line, std::string directive)
{
	if (line.find(".") == std::string::npos || \
		line.find(".") == line.size() - 1)
	{
		std::cout << "Error: " << directive << "argument needs to have: ";
		std::cout << "a name and extension seperated by '.': ";
		std::cout << directive << " <fileName>.<extension>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}
