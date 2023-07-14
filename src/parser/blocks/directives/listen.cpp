#include "parse.hpp"
#include <iostream>
#include <string>

static void	portError(std::string notPort)
{
	std::cout << "Error: incorrect port in configuration file: [" << notPort;
	std::cout << "]: port must be a number in range [0, 65535]" << std::endl;
	exit(EXIT_FAILURE);
}

/**
 * @brief parse a listen command
 * 
 * @param line the std::string to parse the port from
 * @return unsigned short the parsed port number
 */
static unsigned short	parsePort(std::string line)
{
	int	port;

	if (line.size() > 5 || !allDigits(line))
		portError(line);
	try
	{
		port = stoi(line);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		portError(line);
	}
	if (port < 0 || port > 65535)
		portError(line);
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

	line = protectedSubstr(line, 6);
	line = ltrim(line);
	if (line == "")
	{
		std::cout << "Error: can't parse listen directive without arguments" << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (line.find(':') != std::string::npos)
	{
		newHost = protectedSubstr(line, 0, line.find(':'));
		line = protectedSubstr(line, line.find(':') + 1);
	}
	else
		newHost = "0.0.0.0";
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