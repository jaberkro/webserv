#include "parse.hpp"
#include <string>
#include <iostream>

t_values		parseRoot(std::string line, t_values values)
{
	std::string reason = "needs one argument: root <path>;";

	line = protectedSubstr(line, 4);
	line = ltrim(line);
	checkEmptyString(line, "root", reason);
	checkOneArgumentOnly(line, "root");
	checkNotPreviousDirectory(line, "root");
	if ((line.find("/") != 0 && line.find("\"") != 0) || \
	(line.find("\"") == 0 && line.size() > 1 && \
	(line.at(1) != '\"' || line.size() != 2)))
	{
		rootError("path should start with '/'", line);
	}
	if (line.find("/") == 0)
		line = protectedSubstr(line, 1, line.size() - 1);
	if (line.size() > 0 && line.find_last_of("/") == line.size() - 1)
		rootError("path should not end with '/'", line);
	if (line == "")
		line = ".";
	values.root = line;
	return (values);
}
