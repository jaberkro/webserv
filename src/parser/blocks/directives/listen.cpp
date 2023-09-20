#include "parse.hpp"
#include <string>
#include <iostream>

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
