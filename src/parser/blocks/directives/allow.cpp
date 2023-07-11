#include "parse.hpp"
#include <iostream>

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

t_values	parseAllow(std::string line, t_values values)
{
	line = protectedSubstr(line, 5);
	line = ltrim(line);
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		if (isAllowedMethod(protectedSubstr(line, 0, findFirstWhitespace(line))))
		{
			values.allowed.push_back(protectedSubstr(line, 0, findFirstWhitespace(line)));
			line = protectedSubstr(line, findFirstWhitespace(line) + 1);
			line = ltrim(line);
		}
		else
		{
			std::cout << "Error: can't parse allow: invalid method: [" << \
			protectedSubstr(line, 0, findFirstWhitespace(line)) << \
			"]. Allowed methods are: GET, POST, DELETE" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (line != "")
	{
		if (isAllowedMethod(line))
		{
			values.allowed.push_back(line);
		}
		else
		{
			std::cout << "Error: can't parse allow: invalid method: [" << \
			line << "]. Allowed methods are: GET, POST, DELETE" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return (values);
}
