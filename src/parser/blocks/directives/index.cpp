#include "parse.hpp"
#include <iostream>

static void checkExtension(std::string index)
{
	if (index.find(".") == std::string::npos || index.find(".") == index.size() - 1)
	{
		std::cout << "Error: index argument needs to have a name and extension seperated by '.': index <fileName>.<extension>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}

static void checkEmptyString(std::string line)
{
	if (line == "")
	{
		std::cout << "Error: index needs at least one argument: index <fileName>.<extension>;" << std::endl;
		exit(EXIT_FAILURE);
	}
}

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
	checkEmptyString(line);
	while (firstWhitespace(line) != line.size() && firstWhitespace(line) != 0)
	{
		std::string newIndex = protectedSubstr(line, 0, firstWhitespace(line));
		checkExtension(newIndex);
		if (newIndex.find("/") != 0)
			values.indexes.push_back("/" + newIndex);
		else
			values.indexes.push_back(newIndex);
		line = protectedSubstr(line, firstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
	{
		checkExtension(line);
		if (line.find("/") != 0)
			values.indexes.push_back("/" + line);
		else
			values.indexes.push_back(line);
	}
	return (values);
}
