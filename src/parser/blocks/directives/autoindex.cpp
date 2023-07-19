#include "parse.hpp"
#include <iostream>

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
	if (findFirstWhitespace(line) != line.size())
	{
		std::cout << "Error: can't parse autoindex: too many arguments: [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.size() == 2 && line == "on")
		values.autoindex = true;
	else if (line.size() == 3 && line == "off")
		values.autoindex = false;
	else
	{
		std::cout << "Error: can't parse autoindex: value should be 'on' or 'off': [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (values);
}
