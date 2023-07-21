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
	std::string	key;

	line = protectedSubstr(line, 10);
	line = ltrim(line);
	// values.errorPages.clear(); //outcomment this if error_page should overwrite itself
	value = protectedSubstr(line, line.find_last_of(" \t") + 1, line.size() - line.find_last_of(" \t") + 1);
	if (value.find("/") != 0 || value.find(".") == std::string::npos || value.find(".") == value.size() - 1)
	{
		std::cout << "Error: can't parse error_page: [" << value << "]: page should start with '/' and contain extension: /<name>.<extension" << std::endl;
		exit(EXIT_FAILURE);
	}
	line = protectedSubstr(line, 0, line.find_last_of(" \t") + 1);
	line = rtrim(line);
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		key = protectedSubstr(line, 0, findFirstWhitespace(line));
		if (!allDigits(key))
		{
			std::cout << "Error: can't parse error_page: [" << key << "]: not a number" << std::endl;
			exit(EXIT_FAILURE);
		}
		values.errorPages[stoi(protectedSubstr(line, 0, findFirstWhitespace(line)))] = value; // try catch block for stoi needed
		line = protectedSubstr(line, findFirstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
	{
		if (!allDigits(line))
		{
			std::cout << "Error: can't parse error_page: [" << line << "]: not a number" << std::endl;
			exit(EXIT_FAILURE);
		}
		values.errorPages[stoi(line)] = value; // catch possible exception
	}
	return (values);
}
