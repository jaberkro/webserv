#include "parse.hpp"
#include <string>
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

t_values	parseDeny(std::string line, t_values values)
{
	std::string reason = "needs at least one argument: deny <method>;";
	std::string newMethod;

	line = protectedSubstr(line, 4);
	line = ltrim(line);
	checkEmptyString(line, "deny", reason);
	while (firstWhitespace(line) != line.size() && firstWhitespace(line) != 0)
	{
		newMethod = protectedSubstr(line, 0, firstWhitespace(line));
		if (!isAllowedMethod(newMethod))
			methodError(newMethod, "deny", "GET, POST, DELETE, all");
		values.denied.push_back(newMethod);
		line = protectedSubstr(line, firstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
	{
		if (!isAllowedMethod(line))
			methodError(line, "deny", "GET, POST, DELETE, all");
		values.denied.push_back(line);
	}
	return (values);
}
