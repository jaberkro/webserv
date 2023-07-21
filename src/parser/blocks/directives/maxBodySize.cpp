#include "parse.hpp"
#include <string>
#include <iostream>

/**
 * @brief parse a client_max_body_size directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed client_max_body_size
 */
t_values		parseMaxBodySize(std::string line, t_values values)
{
	int	multiplier = 1;

	line = protectedSubstr(line, 20);
	line = ltrim(line);
	if (line == "")
	{
		std::cout << "Error: client_max_body_size needs one argument: client_max_body_size <size>;" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (findFirstWhitespace(line) != line.size())
	{
		std::cout << "Error: can't parse client_max_body_size: too much arguments: [" << line << "]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!allDigits(protectedSubstr(line, 0, line.size() - 1)))
	{
		std::cout << "Error: can't parse client_max_body_size: [" << line << "]: not a number" << std::endl;
		exit(EXIT_FAILURE);
	}
	if ((line.back() == 'k' || line.back() == 'K') && line.size() > 1)
	{
		line.pop_back();
		multiplier = 1000;
	}
	else if ((line.back() == 'm' || line.back() == 'M') && line.size() > 1)
	{
		line.pop_back();
		multiplier = 1000000;
	}
	else if (!isdigit(line.back()))
	{
		std::cout << "Error: can't parse client_max_body_size: [" << line << "]: not a number" << std::endl;
		exit(EXIT_FAILURE);
	}
	try
	{
		if ((line.size() > 3 && multiplier == 1000000) || (line.size() > 6 && multiplier == 1000) || line.size() > 9)
		{
			std::cout << "Error: can't parse client_max_body_size: [" << line << "] * [" << multiplier << "]: input number too big: max 100 megabytes" << std::endl;
			exit(EXIT_FAILURE);
		}
		values.maxBodySize = stoull(line) * multiplier;
		if (values.maxBodySize > 100000000)
		{
			std::cout << "Error: can't parse client_max_body_size: [" << values.maxBodySize << "]: input number too big: max 100 megabytes" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	return (values);
}
