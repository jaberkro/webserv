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

	while (i < s.size() - 1 && (s.at(i) == '\t' || s.at(i) == ' ' || s.at(i) == '\v' || s.at(i) == '\b' || s.at(i) == '\r'))
	{
		i++;
	}
	if (i == s.size() - 1)
		return ("");
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
	return (1);
}
