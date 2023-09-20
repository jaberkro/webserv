#include "parse.hpp"
#include <fstream>
#include <string>
#include <iostream>

int	getValidLine(std::fstream &file, std::string &line)
{
	if (!std::getline(file, line))
		return (0);
	if (line.find('#') != std::string::npos)
		line = protectedSubstr(line, 0, line.find('#'));
	line = ltrim(line);
	line = rtrim(line);
	if (line == "")
		return (1);
	if (line.back() != ';' && line.back() != '{' && line.back() != '}')
	{
		std::cerr << "Error: [" << line << "]: lines in the configuration";
		std::cerr << " file should end with ';', '{' or '}'" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.back() == ';')
	{
		line.pop_back();
		line = rtrim(line);
	}
	return (1);
}

size_t	firstWhitespace(std::string line)
{
	if (line == "")
		return (0);
	if (line.find(" ") == std::string::npos && \
		line.find("\t") == std::string::npos && \
		line.find("\v") == std::string::npos && \
		line.find("\b") == std::string::npos)
		return (line.size());
	return (line.find_first_of(" \t\v\b"));
}

bool	allDigits(std::string s)
{
	for (size_t i = 0; i < s.size(); i++)
	{
		if (!isdigit(s.at(i)))
			return (0);
	}
	return (1);
}

int	parseErrorCode(std::string code, std::string directive)
{
	int newCode;

	if (!allDigits(code))
		nanError(code, directive);
	if (code.size() > 3)
		tooBigError(code, directive, "600");
	try
	{
		newCode = stoi(code);
		if (newCode < 100 || newCode > 600)
		{
			std::cerr << "Error: can't parse " << directive;
			std::cerr << ": invalid code: [" << code << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}
	return (newCode);
}

std::string convertToLower(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (isalpha(str.at(i)))
			str.at(i) = tolower(str.at(i));
	}
	return (str);
}
