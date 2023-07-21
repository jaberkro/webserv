#include "parse.hpp"
#include <iostream>

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
		std::cout << "Error: deny needs at least one argument: deny <method>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}

t_values	parseDeny(std::string line, t_values values)
{
	line = protectedSubstr(line, 4);
	line = ltrim(line);
	checkEmptyString(line);
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		if (isAllowedMethod(protectedSubstr(line, 0, findFirstWhitespace(line))))
		{
			values.denied.push_back(protectedSubstr(line, 0, findFirstWhitespace(line)));
			line = protectedSubstr(line, findFirstWhitespace(line) + 1);
			line = ltrim(line);
		}
		else
		{
			std::cout << "Error: can't parse deny: invalid method: [" << \
			protectedSubstr(line, 0, findFirstWhitespace(line)) << \
			"]. Allowed methods are: GET, POST, DELETE, all" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (line != "")
	{
		if (isAllowedMethod(line))
		{
			values.denied.push_back(line);
		}
		else
		{
			std::cout << "Error: can't parse deny: invalid method: [" << \
			line << "]. Allowed methods are: GET, POST, DELETE, all" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return (values);
}
