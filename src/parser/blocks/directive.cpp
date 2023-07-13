#include "parse.hpp"
#include <string>

/**
 * @brief switch case executing inheritance directive functions
 * 
 * @param directive the case to execute
 * @param line the std::string to be parsed
 * @param values the struct with values to be updated
 * @return t_values the updated struct with inheritance values
 */
t_values	parseInheritanceDirective(int directive, std::string line, t_values values)
{
	switch(directive)
	{
		case 0:
			values = parseRoot(line, values);
			break;
		case 1:
			values = parseIndex(line, values);
			break;
		case 2:
			values = parseAutoindex(line, values);
			break;
		case 3:
			values = parseMaxBodySize(line, values);
			break;
		case 4:
			values = parseErrorPage(line, values);
			break;
	}
	return (values);
}

/**
 * @brief checks if an std::string starts with one of the inheritance directives
 * 
 * @param line the std::string to check
 * @return int the index of the inheritance directives that is at the start of the std::string. If there is no match, -1 is returned
 */
int	hasInheritanceDirective(std::string line)
{
	std::string directives[] = {"root", "index", "autoindex", "client_max_body_size", "error_page"};

	int i = 0;

	while (i < 5)
	{
		if (line.find(directives[i]) == 0)
			return (i);
		i++;
	}
	return (-1);
}