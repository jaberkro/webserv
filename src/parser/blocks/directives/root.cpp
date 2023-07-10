#include "parse.hpp"
#include <string>
#include <iostream>

/**
 * @brief parse a root directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed root
 */
t_values		parseRoot(std::string line, t_values values)
{
	line = protectedSubstr(line, 4);
	line = ltrim(line);
	if (findFirstWhitespace(line) != line.size())
	{
		std::cout << "Error: can't parse autoindex: too much arguments: [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	values.root = line;
	return (values);
}
