#include "parse.hpp"
#include <iostream>

static void methodError(std::string line)
{
	std::cout << "Error: can't parse allow: invalid method: [" << line << \
		"]. Allowed methods are: GET, POST, DELETE" << std::endl;
	exit(EXIT_FAILURE);
}

static t_values	addAllow(std::string &line, t_values values)
{
	values.allowed.push_back(protectedSubstr(line, 0, firstWhitespace(line)));
	line = protectedSubstr(line, firstWhitespace(line) + 1);
	line = ltrim(line);
	return (values);
}

static bool	isAllowedMethod(std::string toCheck)
{
	std::string directives[] = {"GET", "POST", "DELETE"};
	int i = 0;

	while (i < 3)
	{
		if (toCheck.find(directives[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}

static void checkEmptyString(std::string line)
{
	if (line == "")
	{
		std::cout << "Error: allow needs at least one argument: allow <method>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}

t_values	parseAllow(std::string line, t_values values)
{
	line = protectedSubstr(line, 5);
	line = ltrim(line);
	checkEmptyString(line);
	while (firstWhitespace(line) != line.size() && line != "" && firstWhitespace(line) != 0)
	{
		if (!isAllowedMethod(protectedSubstr(line, 0, firstWhitespace(line))))
			methodError(protectedSubstr(line, 0, firstWhitespace(line)));
		values = addAllow(line, values);
	}
	if (line != "")
	{
		if (!isAllowedMethod(line))
			methodError(line);			
		values = addAllow(line, values);
	}
	return (values);
}
