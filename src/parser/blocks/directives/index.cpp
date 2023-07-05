#include "parse.hpp"
#include <iostream>

/**
 * @brief parse an index directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed index appended to the already existing index
 */
t_values	parseIndex(std::string &line, t_values values)
{
	line = protectedSubstr(line, 5);
	line = ltrim(line);
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		values.indexes.push_back(protectedSubstr(line, 0, findFirstWhitespace(line)));
		line = protectedSubstr(line, findFirstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
		values.indexes.push_back(line);
	return (values);
}
