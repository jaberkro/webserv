#include "parse.hpp"
#include <string>
#include <iostream>

static void checkNoEndingSlash(std::string line)
{
	if (line.find_last_of("/") == line.size() - 1)
	{
		std::cout << "Error: can't parse upload_dir: ";
		std::cout << "path should not end with '/': ";
		std::cout << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

static void checkStartingSlash(std::string line)
{
	if (line.find("/") != 0)
	{
		std::cout << "Error: can't parse upload_dir: ";
		std::cout << "path should start with '/': ";
		std::cout << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

static void checkOneArgumentOnly(std::string line)
{
	if (firstWhitespace(line) != line.size())
	{
		std::cout << "Error: can't parse upload_dir: too many arguments: ";
		std::cout << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

static void checkEmptyString(std::string line)
{
	if (line == "")
	{
		std::cout << "Error: upload_dir needs one argument: ";
		std::cout << "upload_dir <path>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief parse a upload_dir directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed upload_dir
 */
t_values	parseUploadDir(std::string line, t_values values)
{
	line = protectedSubstr(line, 10);
	line = ltrim(line);
	checkEmptyString(line);
	checkOneArgumentOnly(line);
	checkStartingSlash(line);
	line = protectedSubstr(line, 1, line.size() - 1);
	checkNoEndingSlash(line);
	values.uploadDir = line;
	return (values);
}
