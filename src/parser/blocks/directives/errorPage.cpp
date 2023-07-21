#include "parse.hpp"
#include <iostream>

static void codeError(std::string code)
{
	std::cout << "Error: can't parse error_page: [" << code << "]: not a valid error code" << std::endl;
	exit(EXIT_FAILURE);
}

static t_values addErrorPage(std::string code, std::string file, t_values values)
{
	try 
	{
		if (!allDigits(code) || code.size() != 3 || !validErrorCode(stoi(code)))
			codeError(code);
		values.errorPages[stoi(code)] = file;
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	return (values);
}

static void checkValidFilename(std::string filename)
{
	if (filename.find("/") != 0 || filename.find(".") == std::string::npos || \
		filename.find(".") == filename.size() - 1)
	{
		std::cout << "Error: can't parse error_page: [" << filename << \
			"]: filename should start with '/' and contain extension: /<name>.<extension" << std::endl;
		exit(EXIT_FAILURE);
	}
}

static void checkEmptyString(std::string line)
{
	if (line == "")
	{
		std::cout << "Error: error_page needs at least two arguments: error_page <code> /<filename>;" << std::endl;
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
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		values = addErrorPage(protectedSubstr(line, 0, findFirstWhitespace(line)), filename, values);
		line = protectedSubstr(line, findFirstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
		values = addErrorPage(line, filename, values);
	return (values);
}
