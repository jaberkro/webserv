#include <string>
#include <fstream>
#include <iostream>

/**
 * @brief substr that catches exception, prints it and sends back "" in case an excption occured
 * 
 * @param s the string to make a substring from
 * @param start the index to start the substring from
 * @return std::string 
 */
std::string protectedSubstr(std::string s, size_t start)
{
	std::string	newString;

	if (s == "")
		return ("");
	try
	{
		newString = s.substr(start);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return ("");
	}
	return (newString);
}

/**
 * @brief substr that catches exception, prints it and sends back "" in case an excption occured
 * 
 * @param s the string to make a substring from
 * @param start the index to start the substring from
 * @param size the amount of characters to include in the new substring
 * @return std::string 
 */
std::string protectedSubstr(std::string s, size_t start, size_t size)
{
	std::string	newString;

	if (s == "")
		return ("");
	try
	{
		newString = s.substr(start, size);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return ("");
	}
	return (newString);
}

/**
 * @brief remove whitespace at beginning of an std::string
 * 
 * @param s the std::string to be trimmed
 * @return the trimmed std::string
 */
std::string	ltrim(std::string s)
{
	size_t i = 0;

	if (s == "")
		return ("");
	while (i < s.size() && (s.at(i) == '\t' || s.at(i) == ' ' || s.at(i) == '\v' || s.at(i) == '\b' || s.at(i) == '\r'))
	{
		i++;
	}
	if (i == s.size())
		return ("");
	else if (i == 0)
		return (s);
	return (protectedSubstr(s, i));
}

/**
 * @brief remove whitespace at the end of an std::string
 * 
 * @param s the std::string to be trimmed
 * @return the trimmed std::string
 */
std::string	rtrim(std::string s)
{
	size_t i = s.size() - 1;

	if (s == "")
		return ("");
	while (i >= 0 && (s.at(i) == '\t' || s.at(i) == ' ' || s.at(i) == '\v' || s.at(i) == '\b' || s.at(i) == '\r'))
	{
		i--;
	}
	if (i < 0)
		return ("");
	else if (i == s.size() - 1)
		return (s);
	return (protectedSubstr(s, 0, i + 1));
}

/**
 * @brief getline that checks the line for valid input, trims whitespaces at the beginning and end of the string and removes comments
 * 
 * @param file the file to read the next line from
 * @param line the std::string to store the next line in
 * @return int 1 when getline worked, 0 when the line is empty
 */
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
	// std::cout << line << std::endl;
	if (line.back() != ';' && line.back() != '{' && line.back() != '}')
	{
		std::cout << "Error: missing ';', '{' or '}' in configuration file" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.back() == ';')
	{
		line.pop_back();
		line = rtrim(line);
	}
	return (1);
}

/**
 * @brief find the first space or horizontal tab
 * 
 * @return size_t the index of the first space or horizontal tab found, 
 * size of string if no tab or space was found, 0 if line is empty
 */
size_t	findFirstWhitespace(std::string line)
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

/**
 * @brief find the last space or horizontal tab
 * 
 * @return size_t the index of the last space or horizontal tab found, 
 * size of string if no tab or space was found, 0 if line is empty
 */
size_t	findLastWhitespace(std::string line)
{
	if (line == "")
		return (0);
	if (line.find_last_of(" ") == std::string::npos && line.find_last_of("\t") == std::string::npos)
		return (line.size());
	else if (line.find_last_of(" ") == std::string::npos)
		return (line.find_last_of("\t"));
	else if (line.find_last_of("\t") == std::string::npos)
		return (line.find_last_of(" "));
	return (std::max(line.find_last_of(" "), line.find_last_of("\t")));
}

bool	allDigits(std::string s)
{
	for (size_t i = 0; i < s.size(); i++)
	{
		if (!isdigit(s.at(i)))
		{
			return (0);
		}
	}
	return (1);
}
