#include "parse.hpp"
#include <iostream>

/**
 * @brief parse an index directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed index appended to the already existing index
 */
t_values	parseIndex(std::string line, t_values values)
{
	line = protectedSubstr(line, 5);
	line = ltrim(line);
	if (line == "")
	{
		std::cout << "Error: index needs at least one argument: index <fileName>.<extension>;" << std::endl;
		exit(EXIT_FAILURE);
	}
	while (findFirstWhitespace(line) != line.size() && line != "" && findFirstWhitespace(line) != 0)
	{
		std::string newIndex = protectedSubstr(line, 0, findFirstWhitespace(line));
		if (newIndex.find(".") == std::string::npos || newIndex.find(".") == newIndex.size() - 1)
		{
			std::cout << "Error: index argument needs to have a name and extension seperated by '.': index <fileName>.<extension>;" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (newIndex.find("/") != 0)
			values.indexes.push_back("/" + newIndex);
		else
			values.indexes.push_back(newIndex);
		line = protectedSubstr(line, findFirstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
	{
		if (line.find(".") == std::string::npos || line.find(".") == line.size() - 1)
		{
			std::cout << "Error: index argument needs to have a name and extension seperated by '.': index <fileName>.<extension>;" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (line.find("/") != 0)
			values.indexes.push_back("/" + line);
		else
			values.indexes.push_back(line);
	}
	return (values);
}
