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
	std::string reason = "needs one argument: root <path>;";

	line = protectedSubstr(line, 4);
	line = ltrim(line);
	checkEmptyString(line, "root", reason);
	checkOneArgumentOnly(line, "root");
	if ((line.find("/") != 0 && line.find("\"") != 0) || \
		(line.find("\"") == 0 && line.size() > 1 && \
		(line.at(1) != '\"' || line.size() != 2)))
	{
		std::cerr << "Error: can't parse root: path should start with '/': ";
		std::cerr << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.find("/") == 0)
		line = protectedSubstr(line, 1, line.size() - 1);
	if (line.find_last_of("/") == line.size() - 1)
	{
		std::cerr << "Error: can't parse root: path should not end with '/': ";
		std::cerr << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	values.root = line;
	return (values);
}
