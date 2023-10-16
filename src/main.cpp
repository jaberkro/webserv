#include "parse.hpp"
#include "Webserver.hpp"
#include <iostream>

int	main(int argc, char **argv)
{
	std::vector<Server> servers;
	std::string 		defaultFile = "config/default.conf";

	if (argc > 2)
	{
		std::cerr << "usage: ./webserv [configuration file]" << std::endl;
		return (1);
	}
	else if (argc == 1)
		parse(servers, defaultFile.c_str());
	else
		parse(servers, argv[1]);
	// try
	// {
	// 	// printServers(servers); // turn on to see parsed configuration settings
	// } 
	// catch (const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// }
	try
	{
		Webserver	webserv(servers);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
