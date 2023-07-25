#include "parse.hpp"
#include <iostream>

static void checkEmptyString(std::string line)
{
	if (line == "")
	{
		std::cout << "Error: autoindex needs one argument: ";
		std::cout << "autoindex <value>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief parse an autoindex directive
 * 
 * @param line the line to parse
 * @param values this struct contains the autoindex value to be overwritten
 * @return t_values the updated struct containing the parsed autoindex
 */
t_values	parseAutoindex(std::string line, t_values values)
{
	line = protectedSubstr(line, 9);
	line = ltrim(line);
	checkEmptyString(line);
	if (firstWhitespace(line) != line.size())
	{
		std::cout << "Error: can't parse autoindex: too many arguments: ";
		std::cout << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.size() == 2 && line == "on")
		values.autoindex = true;
	else if (line.size() == 3 && line == "off")
		values.autoindex = false;
	else
	{
		std::cout << "Error: can't parse autoindex: ";
		std::cout << "value should be 'on' or 'off': ";
		std::cout << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (values);
}
