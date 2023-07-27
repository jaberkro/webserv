#include "parse.hpp"
#include <string>

/**
 * @brief switch case executing directive parsing for directives 
 * that can be found in all blocks (http, server, location)
 * 
 * @param directive the case to execute
 * @param line the std::string to be parsed
 * @param values the struct with values to be updated
 * @return t_values the updated struct with inheritance values.
 */
t_values	parseDirective(int directive, std::string line, t_values values)
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
		case 5:
			values = parseReturn(line, values);
			break;
		case 6:
			values = parseUploadDir(line, values);
			break;
	}
	return (values);
}

/**
 * @brief checks if an std::string starts with one of the directives that can 
 * be foudn in all blocks
 * 
 * @param line the std::string to check
 * @return int the index of the inheritance directive that is at the start of 
 * the std::string. If there is no match, -1 is returned
 */
int	hasDirective(std::string line)
{
	std::string directives[] = {"root", "index", "autoindex", \
		"client_max_body_size", "error_page", "return", "upload_dir"};

	int i = 0;

	while (i < 7)
	{
		if (line.find(directives[i]) == 0)
			return (i);
		i++;
	}
	return (-1);
}

/**
 * @brief switch case executing directive parsing functions that can be found
 * only inside a location block only
 * 
 * @param directive the case to execute
 * @param line the std::string to be parsed
 * @param values the struct with values to be updated
 * @return t_values the updated struct with inheritance values
 */
t_values	parseLocDirective(int directive, std::string line, t_values values)
{
	switch(directive)
	{
		case 0:
			values = parseAllow(line, values);
			break;
		case 1:
			values = parseDeny(line, values);
			break;
	}
	return (values);
}

/**
 * @brief checks if an std::string starts with one of the location directives
 * 
 * @param line the std::string to check
 * @return int the index of the location directive that is at the start of 
 * the std::string. If there is no match, -1 is returned.
 */
int	hasLocDirective(std::string line)
{
	std::string directives[] = {"allow", "deny"};

	int i = 0;

	while (i < 2)
	{
		if (line.find(directives[i]) == 0)
			return (i);
		i++;
	}
	return (-1);
}
