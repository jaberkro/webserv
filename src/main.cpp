#include "webserv.hpp"
#include <iostream>
#include "Webserver.hpp"
#include <map>

void	printServers(std::vector<Server> &servers)
{
	std::cout << std::endl;
	std::cout << "config:" << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "\tserver " << i << ":";
		for (size_t j = 0; j < servers.at(i).getListens().size(); j++)
		{
			std::cout << "\n\t\thost: " << servers.at(i).getHost(j) << "\t\t";
			std::cout << "port: " << servers.at(i).getPort(j);
		}
		if (servers.at(i).getServerNames().size() > 0)
			std::cout << "\n\t\tserver_name:";
		for (size_t j = 0; j < servers.at(i).getServerNames().size(); j++)
		{
			std::cout << " " << servers.at(i).getServerName(j);
		}
		for (size_t j = 0; j < servers.at(i).getLocations().size(); j++)
		{
			std::cout << "\n\t\tlocation:\n\t\t\tmodifier: ";
			std::cout << servers.at(i).getLocation(j).getModifier();
			std::cout << "\n\t\t\tmatch: ";
			std::cout << servers.at(i).getLocation(j).getMatch();
			std::cout << "\n\t\t\troot: ";
			std::cout << servers.at(i).getLocation(j).getRoot();
			std::cout << "\n\t\t\tindex:";
			for (size_t k = 0; k < servers.at(i).getLocation(j).getIndexes().size(); k++)
			{
				std::cout << " " << servers.at(i).getLocation(j).getIndex(k);
			}
			if (servers.at(i).getLocation(j).getAutoindex() == true)
				std::cout << "\n\t\t\tautoindex: on";
			else
				std::cout << "\n\t\t\tautoindex: off";
			std::cout << "\n\t\t\tclient_max_body_size: ";
			std::cout << servers.at(i).getLocation(j).getMaxBodySize();
			std::map<int, std::string> map = servers.at(i).getLocation(j).getErrorPages();
			if (!map.empty())
			{
				std::cout << "\n\t\t\terror_page: ";
				for (std::map<int, std::string>::iterator it= map.begin(); it!=map.end(); ++it)
					std::cout << "\n\t\t\t\t" << it->first << " => " << it->second;
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int	main(int argc, char **argv)
{
	std::vector<Server> servers;

	if (argc != 2)
	{
		std::cout << "usage: ./webserv [configuration file]" << std::endl;
		return (1);
	}

	parse(servers, argv[1]);
	printServers(servers);

	std::cout << "Hello World!" << std::endl;
	try {
		Webserver	webserv(servers);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
