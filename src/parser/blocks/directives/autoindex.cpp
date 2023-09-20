#include "parse.hpp"
#include <string>
#include <iostream>

t_values	parseAutoindex(std::string line, t_values values)
{
	std::string reason = "needs one argument: autoindex <on/off>;";
	line = protectedSubstr(line, 9);
	line = ltrim(line);
	checkEmptyString(line, "autoindex", reason);
	if (firstWhitespace(line) != line.size())
	{
		std::cerr << "Error: can't parse autoindex: too many arguments: ";
		std::cerr << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.size() == 2 && line == "on")
		values.autoindex = true;
	else if (line.size() == 3 && line == "off")
		values.autoindex = false;
	else
	{
		std::cerr << "Error: can't parse autoindex: ";
		std::cerr << "value should be 'on' or 'off': ";
		std::cerr << "[" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (values);
}
