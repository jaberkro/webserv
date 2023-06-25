#include "webserv.hpp"
#include <iostream>

void	printConfig(Config &config)
{
	std::cout << std::endl;
	std::cout << "config:" << std::endl;
	for (size_t i = 0; i < config.getServers().size(); i++)
	{
		std::cout << "\tserver " << i << ":";
		for (size_t j = 0; j < config.getServer(i).getListens().size(); j++)
		{
			std::cout << "\n\t\thost: " << config.getServer(i).getHost(j) << "\t\t";
			std::cout << "port: " << config.getServer(i).getPort(j);
		}
		if (config.getServer(i).getServerNames().size() > 0)
			std::cout << "\n\t\tserver_name:";
		for (size_t j = 0; j < config.getServer(i).getServerNames().size(); j++)
		{
			std::cout << " " << config.getServer(i).getServerName(j);
		}
		// std::cout << std::endl;
		for (size_t j = 0; j < config.getServer(i).getLocations().size(); j++)
		{
			std::cout << "\n\t\tlocation:\n\t\t\tmodifier: ";
			std::cout << config.getServer(i).getLocation(j).getModifier();
			std::cout << "\n\t\t\tmatch: ";
			std::cout << config.getServer(i).getLocation(j).getMatch();

		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


int	main(int argc, char **argv)
{
	Config config;

	if (argc != 2)
	{
		std::cout << "usage: ./webserv [configuration file]" << std::endl;
		return (1);
	}

	parse(config, argv[1]);
	printConfig(config);
	return (0);
}
