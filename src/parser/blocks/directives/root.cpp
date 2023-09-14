#include "parse.hpp"
#include <string>
#include <iostream>

/**
 * @brief parse a root directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed root
 */
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
		(line.at(1) != '\"' || line.size() != 2))) // is this check still needed? root ""; is not allowed right? should be root /;
	{
		rootError("path should start with '/'", line);
	}
	if (line.find("/") == 0) // is this check still needed? is both root data; and root /data; allowed?
		line = protectedSubstr(line, 1, line.size() - 1);
	if (line.size() > 0 && line.find_last_of("/") == line.size() - 1)
		rootError("path should not end with '/'", line);
	if (line == "")
		line = ".";
	values.root = line;
	return (values);
}
