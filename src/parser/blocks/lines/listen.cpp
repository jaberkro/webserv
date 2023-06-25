#include "parse.hpp"
#include <iostream>
#include <string>

/**
 * @brief parse a listen command
 * 
 * @param line the std::string to parse the port from
 * @return unsigned short the parsed port number
 */
static unsigned short	parsePort(std::string line)
{
	int	port;

	if (line.size() > 5)
	{
		std::cout << "ERROR: incorrect port in configuration file: port must be in range [0,65535]" << std::endl;
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < line.size(); i++)
	{
		if (!isdigit(line.at(i)))
		{
			std::cout << "ERROR: incorrect port in configuration file: [" << line << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	try
	{
		port = stoi(line);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cout << "ERROR: incorrect port in configuration file: port must be in range [0,65535]" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (port < 0 || port > 65535)
	{
		std::cout << "ERROR: incorrect port in configuration file: port must be in range [0,65535]" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (port);
}

/**
 * @brief parse a listen command
 *  use first half of std::string to parse the host, 
 *  save last part for parsePort() function
 * 
 * @param line the line that contains the host and port
 * @return std::string the hostname
 */
static std::string parseHost(std::string &line)
{
	std::string	newHost;

	if (line.find(':') != std::string::npos)
	{
		line = line.substr(6);
		line = ltrim(line);
		newHost = line.substr(0, line.find(':'));
		line = line.substr(line.find(':') + 1);
	}
	else
	{
		newHost = "0.0.0.0";
		line = line.substr(6);
		line = ltrim(line);
	}
	return (newHost);
}

/**
 * @brief parse a listen line
 * 
 * @param line the string to find a host and port in
 * @return std::pair<std::string, unsigned short> a pair containing the host and port of this listen line
 */
std::pair<std::string, unsigned short> parseListen(std::string line)
{
	std::pair<std::string, unsigned short> newListen(parseHost(line), parsePort(line));

	return (newListen);
}