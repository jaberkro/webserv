#include "parse.hpp"
#include <iostream>

bool	parseAutoindex(std::string &line)
{
	line = protectedSubstr(line, 9);
	line = ltrim(line);
	if (findFirstWhitespace(line) != line.size())
	{
		std::cout << "Error: can't parse autoindex: too much arguments: [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.size() == 2 && line == "on")
		return (true);
	else if (line.size() == 3 && line == "off")
		return (false);
	std::cout << "Error: can't parse autoindex: value should be 'on' or 'off': [" << line << "]" << std::endl;
	exit(EXIT_FAILURE);
	return (false);
}
