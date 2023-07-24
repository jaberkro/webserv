#include "parse.hpp"
#include <iostream>

static void checkValidFilename(std::string filename)
{
	if (filename.find("/") != 0 || filename.find(".") == std::string::npos || \
		filename.find(".") == filename.size() - 1)
	{
		std::cout << "Error: can't parse error_page: [" << filename << "]: ";
		std::cout << "filename should start with '/' and contain extension: ";
		std::cout << "<name>.<extension" << std::endl;
		exit(EXIT_FAILURE);
	}
}

static void checkEmptyString(std::string line)
{
	if (line == "")
	{
		std::cout << "Error: error_page needs at least two arguments: ";
		std::cout << "error_page <code> /<filename>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief parse an error_page directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed error_page 
 * overwriting the already existing error_page
 */
t_values	parseErrorPage(std::string line, t_values values)
{
	std::string	filename;
	std::string	code;
	size_t		lastWhitespace;

	line = protectedSubstr(line, 10);
	line = ltrim(line);
	checkEmptyString(line);
	// values.errorPages.clear(); //outcomment this if error_page should overwrite itself
	lastWhitespace = line.find_last_of(" \t\v\b") + 1;
	filename = protectedSubstr(line, lastWhitespace, line.size() - lastWhitespace);
	checkValidFilename(filename);
	line = protectedSubstr(line, 0, line.find_last_of(" \t") + 1);
	line = rtrim(line);
	while (firstWhitespace(line) != line.size() && firstWhitespace(line) != 0)
	{
		code = protectedSubstr(line, 0, firstWhitespace(line));
		values.errorPages[parseErrorCode(code, "error_page")] = filename;
		line = protectedSubstr(line, firstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
		values.errorPages[parseErrorCode(line, "error_page")] = filename;
	return (values);
}
