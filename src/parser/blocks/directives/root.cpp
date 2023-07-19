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
	if (line == "")
	{
		std::cout << "Error: root needs one argument: root <path>;" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (findFirstWhitespace(line) != line.size())
	{
		std::cout << "Error: can't parse root: too many arguments: [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if ((line.find("/") != 0 && line.find("\"") != 0) || (line.find("\"") == 0 && line.size() > 1 && (line.at(1) != '\"' || line.size() != 2)))
	{
		std::cout << "Error: can't parse root: path should start with '/': [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.find("/") == 0)
		line = protectedSubstr(line, 1, line.size() - 1);
	if (line.find_last_of("/") == line.size() - 1)
	{
		std::cout << "Error: can't parse root: path should not end with '/': [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	values.root = line;
	return (values);
}
