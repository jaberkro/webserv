#include <string>
#include <fstream>
#include <iostream>

/**
 * @brief remove whitespace at beginning of an std::string
 * 
 * @param s the std::string to be trimmed
 * @return the trimmed std::string
 */
std::string	ltrim(std::string s)
{
	size_t i = 0;

	while (i < s.size() && (s.at(i) == '\t' || s.at(i) == ' ' || s.at(i) == '\v' || s.at(i) == '\b' || s.at(i) == '\r'))
	{
		i++;
	}
	if (i == s.size())
		return ("");
	else if (i == 0)
		return (s);
	return (s.substr(i));
}

/**
 * @brief getline that checks the line for valid input, trims whitespaces at the beginning and removes comments
 * 
 * @param file the file to read the next line from
 * @param line the std::string to store the next line in
 * @return int 1 when getline worked, 0 when the line is empty
 */
int	getValidLine(std::fstream &file, std::string &line)
{
	std::getline(file, line);
	if (line == "")
		return (0);
	line = ltrim(line);
	if (line == "")
		return (0);
	if (line.find('#'))
		line = line.substr(0, line.find('#'));
	if (line.back() != ';' && line.back() != '{' && line.back() != '}')
	{
		std::cout << "Error: missing ';', '{' or '}' in configuration file" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (line.back() == ';')
		line.pop_back();
	line = ltrim(line);
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
	if (line.find(" ") == std::string::npos && line.find("\t") == std::string::npos)
		return (line.size());
	else if (line.find(" ") == std::string::npos)
		return (line.find("\t"));
	else if (line.find("\t") == std::string::npos)
		return (line.find(" "));
	return (std::min(line.find(" "), line.find("\t")));
}