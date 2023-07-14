#include "parse.hpp"
#include <string>
#include <iostream>

t_values	parseReturn(std::string line, t_values values)
{
	std::string code;

	line = protectedSubstr(line, 6);
	line = ltrim(line);
	code = protectedSubstr(line, 0, findFirstWhitespace(line));
	if (code.size() != 3 || !allDigits(code))
	{
		std::cout << "Error: can't parse return: code should be 3 digits: [" << code << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	try
	{
		values.returnCode = stoi(code);
		if (values.returnCode < 200 || values.returnCode > 600) // more elaborate test to check for allowed error codes
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
	values.returnText = line;
	return (values);
}
