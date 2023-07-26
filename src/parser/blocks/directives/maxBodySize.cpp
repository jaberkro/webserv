#include "parse.hpp"
#include <string>
#include <iostream>

static unsigned int convertBodySize(std::string line, std::string directive)
{
	int				multiplier = 1;
	unsigned int	bodySize = 0;

	if ((line.back() == 'k' || line.back() == 'K') && line.size() > 1)
		multiplier = 1000;
	else if ((line.back() == 'm' || line.back() == 'M') && line.size() > 1)
		multiplier = 1000000;
	else if (!isdigit(line.back()))
		nanError(line, directive);
	if ((line.size() > 4 && line.back() == 'm') || \
		(line.size() > 4 && line.back() == 'M') || \
		(line.size() > 7 && line.back() == 'k') || \
		(line.size() > 7 && line.back() == 'K') || \
		line.size() > 9)
	{
		tooBigError(line, directive, "100MB");
	}
	if (multiplier != 1)
		line.pop_back();
	bodySize = stoull(line) * multiplier;
	if (bodySize > 100000000)
		tooBigError(std::to_string(bodySize), directive, "100MB");
	return (bodySize);
}

/**
 * @brief parse a client_max_body_size directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed 
 * client_max_body_size.
 */
t_values		parseMaxBodySize(std::string line, t_values values)
{
	std::string reason = "needs one argument: client_max_body_size <size>;";

	line = protectedSubstr(line, 20);
	line = ltrim(line);
	checkEmptyString(line, "client_max_body_size", reason);
	checkOneArgumentOnly(line, "client_max_body_size");
	if (!allDigits(protectedSubstr(line, 0, line.size() - 1)))
		nanError(line, "client_max_body_size");
	try
	{
		values.maxBodySize = convertBodySize(line, "client_max_body_size");
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	return (values);
}
