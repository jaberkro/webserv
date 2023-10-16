#include "parse.hpp"
#include <iostream>

static unsigned int convertBodySize(std::string line, std::string directive)
{
	int					multiplier = 1;
	unsigned long long	bodySize = 0;

	if ((line.back() == 'k' || line.back() == 'K') && line.size() > 1)
		multiplier = 1000;
	else if ((line.back() == 'm' || line.back() == 'M') && line.size() > 1)
		multiplier = 1000000;
	else if (!isdigit(line.back()))
		nanError(line, directive);
	if (multiplier != 1)
		line.pop_back();
	bodySize = stoull(line) * multiplier;
	if (stoull(line) > bodySize)
		tooBigError(line, "client_max_body_size", "18446744073709551615");
	return (bodySize);
}

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
		if (strcmp(e.what(), "stoull: out of range") == 0)
			tooBigError(line, "client_max_body_size", "18446744073709551615");
		else
			nanError(line,"client_max_body_size");
	}
	return (values);
}
