#include "parse.hpp"
#include <string>
#include <iostream>

static bool quotedArgument(std::string text)
{
	if (text.at(0) != '"' || text.size() == 1 || text.back() != '"')
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
		std::cout << "Error: return needs at least one argument: return <code> [text];" << std::endl;
		exit(EXIT_FAILURE);
	}
}

t_values	parseReturn(std::string line, t_values values)
{
	std::string code;

	line = protectedSubstr(line, 6);
	line = ltrim(line);
	checkEmptyString(line);
	code = protectedSubstr(line, 0, findFirstWhitespace(line));
	if (code.size() != 3 || !allDigits(code))
	{
		std::cout << "Error: can't parse return: code should be 3 digits: [" << code << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	try
	{
		values.returnCode = stoi(code);
		if (!validErrorCode(values.returnCode))
		{
			std::cout << "Error: can't parse return: invalid code: [" << values.returnCode << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}
	line = protectedSubstr(line, findFirstWhitespace(line), line.size() - findFirstWhitespace(line));
	line = ltrim(line);
	if (line.size() > 0 && findFirstWhitespace(line) != line.size() && !quotedArgument(line)) // return 404 "quote" more; is now still accepted
	{
		std::cout << "Error: can't parse return: too many text arguments: [" << line<< "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	values.returnText = line;
	return (values);
}
