#include "Config.hpp"
#include "parse.hpp"
#include <fstream>
#include <iostream>
#include <string>

/**
 * @brief parse HTTP block
 * 
 * @param config the variable to store the parsed information in
 * @param file the file to parse the information from
 */
void parseHTTP(Config &config, std::fstream &file)
{
	std::string line;

	while (getValidLine(file, line))
	{
		if (line == "")
			std::cout << "empty line in http block" << std::endl;
		else if (line == "}")
			break ;
		else if (line == "server {")
		{
			config.addServer(parseServer(file));
		}
		else
		{
			std::cout << "Error: can't parse http block near [" << line << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return ;
}