#include "parse.hpp"
#include <iostream>

static void methodError(std::string line)
{
	std::cout << "Error: can't parse deny: invalid method: [" << line << \
		"]. Allowed methods are: GET, POST, DELETE, all" << std::endl;
	exit(EXIT_FAILURE);
}

static t_values	addDeny(std::string &line, t_values values)
{
	values.denied.push_back(protectedSubstr(line, 0, firstWhitespace(line)));
	line = protectedSubstr(line, firstWhitespace(line) + 1);
	line = ltrim(line);
	return (values);
}

static bool	isAllowedMethod(std::string toCheck)
{
	std::string directives[] = {"GET", "POST", "DELETE", "all"};

	int i = 0;

	while (i < 4)
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
		std::cout << "Error: deny needs at least one argument: ";
		std::cout << "deny <method>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}

t_values	parseDeny(std::string line, t_values values)
{
	line = protectedSubstr(line, 4);
	line = ltrim(line);
	checkEmptyString(line);
	while (firstWhitespace(line) != line.size() && firstWhitespace(line) != 0)
	{
		if (!isAllowedMethod(protectedSubstr(line, 0, firstWhitespace(line))))
			methodError(protectedSubstr(line, 0, firstWhitespace(line)));
		values = addDeny(line, values);
	}
	if (line != "")
	{
		if (!isAllowedMethod(line))
			methodError(line);
		values = addDeny(line, values);
	}
	return (values);
}
