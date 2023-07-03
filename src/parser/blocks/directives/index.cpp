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
	values.indexes.push_back(line);
	return (values);
}
