#include "parse.hpp"
#include <iostream>

/**
 * @brief parse an error_page directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed error_page overwriting the already existing error_page
 */
t_values	parseErrorPage(std::string line, t_values values)
{
	std::string	value;
	line = protectedSubstr(line, 10);
	line = ltrim(line);
	// values.errorPages.clear();
	value = protectedSubstr(line, line.find_last_of(" \t"), line.size() - line.find_last_of(" \t"));
	line = protectedSubstr(line, 0, line.find_last_of(" \t"));
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		//check valid number
		values.errorPages[stoi(protectedSubstr(line, 0, findFirstWhitespace(line)))] = value;
		line = protectedSubstr(line, findFirstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
	{
		//check valid number
		values.errorPages[stoi(line)] = value;
	}
	return (values);
}
