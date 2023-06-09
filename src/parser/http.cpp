#include "Config.hpp"
#include "Server.hpp"
#include "parse.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

/**
 * @brief parse a server_name command
 * 
 * @param serverNames the server_names that were found in this line
 * @param line the std::string to parse the serverNames from
 */
static void parseServerNames(std::vector<std::string> &serverNames, std::string &line)
{
	line = line.substr(12);
	while (line.find(" ") != std::string::npos)
	{
		serverNames.push_back(line.substr(0, line.find(" ")));
		line = line.substr(line.find(" ") + 1);
	}
	serverNames.push_back(line);
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

	if (line.size() > 5)
	{
		std::cout << "ERROR: incorrect port in configuration file: port must be in range [0,65535]" << std::endl;
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < line.size(); i++)
	{
		if (!isdigit(line.at(i)))
		{
			std::cout << "ERROR: incorrect port in configuration file" << std::endl;
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
		newHost = line.substr(7, line.find(':') - 7);
		line = line.substr(line.find(':') + 1);
	}
	else
	{
		newHost = "localhost";
		line = line.substr(7);
	}
	return (newHost);
}

/**
 * @brief parse a server block
 * 
 * @param config the variable to store the parsed information in
 * @param file the file to parse the information from
 */
static void parseServer(Config &config, std::fstream &file)
{
	std::string					line;
	std::vector<unsigned short>	ports;
	std::vector<std::string>	hosts;
	std::vector<std::string>	serverNames;

	while (getValidLine(file, line))
	{
		line = ltrim(line);
		if (line == "}")
			break ;
		else if (line.find("listen ") == 0)
		{
			hosts.push_back(parseHost(line));
			ports.push_back(parsePort(line));
		}
		else if (line.find("server_name") == 0)
		{
			parseServerNames(serverNames, line);
		}
		else
			std::cout << line << std::endl;
	}
	if (ports.size() == 0)
	{
		ports.push_back(80);
		hosts.push_back("localhost");
	}
	Server newServer(ports, hosts, serverNames);
	config.addServer(newServer);
	return ;
}

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
		line = ltrim(line);
		if (line == "}")
			break ;
		else if (line == "server {")
		{
			parseServer(config, file);
		}
	}
	return ;
}