#include "parse.hpp"
#include <string>
#include <iostream>

/**
 * @brief parse a listen command
 * 
 * @param line the std::string to parse the port from
 * @return unsigned short the parsed port number
 */
static unsigned short	parsePort(std::string line)
{
	int	port;

	if (line.size() == 0)
		return (80);
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

static void checkValidHost(std::string host)
{
	int dotCount = 0;
	int	num = 0;

	if (host == "localhost")
		return ;
	for (size_t i = 0; i < host.size(); i++)
	{
		if (host.at(i) == '.')
		{
			if (num > 255)
				hostError(host);
			dotCount++;
			num = 0;
		}
		else if (isdigit(host.at(i)))
		{
			num *= 10;
			num += host.at(i) - 48;
		}
		else
			hostError(host);
	}
	if (num > 255 || dotCount != 3)
		hostError(host);
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
		newHost = protectedSubstr(line, 0, line.find(':'));
		if (newHost == "")
			newHost = "0.0.0.0";
		line = protectedSubstr(line, line.find(':') + 1);
		line = ltrim(line);
		newHost = convertToLower(newHost);
	}
	else if (!allDigits(line))
	{
		newHost = line;
		line = "";
	}
	else
		newHost = "0.0.0.0";
	checkValidHost(newHost);	
	return (newHost);
}

/**
 * @brief parse a listen line
 * 
 * @param line the string to find a host and port in
 * @return std::pair<std::string, unsigned short> a pair containing
 *  the host and port of this listen line.
 */
std::pair<std::string, unsigned short> parseListen(std::string line)
{
	std::string reason = "needs argument(s): listen [host]:[port]";
	std::pair<std::string, unsigned short> newListen;

	line = protectedSubstr(line, 6);
	line = ltrim(line);
	checkEmptyString(line, "listen", reason);
	newListen = make_pair(parseHost(line), parsePort(line));
	return (newListen);
}
