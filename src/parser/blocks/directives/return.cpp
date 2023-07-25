#include "parse.hpp"
#include <string>
#include <iostream>

static bool quotedArgument(std::string text)
{
	if (text.size() < 2 || text.at(0) != '"' || text.back() != '"')
	{
		return (false);
	}
	for (size_t i = 1; i < text.size() - 2; i++)
	{
		if (text.at(i) == '"')
			return (false);
	}
	return (true);
}

static void checkEmptyString(std::string line)
{
	if (line == "")
	{
		std::cout << "Error: return needs at least one argument: ";
		std::cout << "return <code> [text];" << std::endl;
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief parse a return directive and store it in values
 * 
 * @param line the line to parse
 * @param values the values to be updated
 * @return t_values the updated values
 */
t_values	parseReturn(std::string line, t_values values)
{
	std::string	code;
	size_t		firstSpace;

	line = protectedSubstr(line, 6);
	line = ltrim(line);
	checkEmptyString(line);
	firstSpace = firstWhitespace(line);
	code = protectedSubstr(line, 0, firstSpace);
	values.returnCode = parseErrorCode(code, "return");
	line = protectedSubstr(line, firstSpace, line.size() - firstSpace);
	line = ltrim(line);
	if (line.size() > 0 && firstWhitespace(line) != line.size() && \
		!quotedArgument(line))
	{
		std::cout << "Error: can't parse return: too many text arguments: ";
		std::cout << "[" << line<< "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (quotedArgument(line))
	{
		line = protectedSubstr(line, 1, line.size() - 2);
	}
	values.returnText = line;
	return (values);
}
