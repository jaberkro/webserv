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
	std::string reason = "needs at least one argument: allow <method>;";
	std::string	newMethod;

	line = protectedSubstr(line, 5);
	line = ltrim(line);
	checkEmptyString(line, "allow", reason);
	while (firstWhitespace(line) != line.size() && firstWhitespace(line) != 0)
	{
		newMethod = protectedSubstr(line, 0, firstWhitespace(line));
		if (!isAllowedMethod(newMethod))
			methodError(newMethod, "allow", "GET, POST, DELETE");
		values.allowed.push_back(newMethod);
		line = protectedSubstr(line, firstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
	{
		if (!isAllowedMethod(line))
			methodError(line, "allow", "GET, POST, DELETE");	
		values.allowed.push_back(line);
	}
	return (values);
}
