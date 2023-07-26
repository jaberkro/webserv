#include "webserv.hpp"
#include <iostream>
#include "Webserver.hpp"
#include <map>

void	printServers(std::vector<Server> servers)
{
	std::cout << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "server " << i << ":";
		if (!(servers.at(i).getHost(0) == "0.0.0.0" && servers.at(i).getPort(0) == 80))
		{
			for (size_t j = 0; j < servers.at(i).getListens().size(); j++)
			{
				std::cout << "\n\thost: " << servers.at(i).getHost(j) << "\t";
				std::cout << "port: " << servers.at(i).getPort(j);
			}
		}
		if (servers.at(i).getServerNames().size() > 0)
			std::cout << "\n\tserver_name:";
		for (size_t j = 0; j < servers.at(i).getServerNames().size(); j++)
		{
			std::cout << " " << servers.at(i).getServerName(j);
		}


		std::map<int, std::string> errorPages = servers.at(i).getErrorPages();
		if (!errorPages.empty())
		{
			std::cout << "\n\terror_page: ";
			std::cout << "size: " << errorPages.size();
			for (std::map<int, std::string>::iterator sit= errorPages.begin(); sit!=errorPages.end(); ++sit)
				std::cout << "\n\t\t" << sit->first << " => " << sit->second;
		}
		for (size_t j = 0; j < servers.at(i).getLocations().size(); j++)
		{
			std::cout << "\n\tlocation ";
			if (servers.at(i).getLocation(j).getModifier() != "(none)")
				std::cout << servers.at(i).getLocation(j).getModifier() << " ";
			std::cout << servers.at(i).getLocation(j).getMatch() << ":";
			if (servers.at(i).getLocation(j).getRoot().size())
			{
				std::cout << "\n\t\troot: ";
				std::cout << servers.at(i).getLocation(j).getRoot();
			}
			if (servers.at(i).getLocation(j).getIndexes().size())
			{
				std::cout << "\n\t\tindex:";
				for (size_t k = 0; k < servers.at(i).getLocation(j).getIndexes().size(); k++)
				{
					std::cout << " " << servers.at(i).getLocation(j).getIndex(k);
				}
			}
			if (servers.at(i).getLocation(j).getAutoindex() == true)
				std::cout << "\n\t\tautoindex: on";
			if (servers.at(i).getLocation(j).getMaxBodySize() != 1000000)
			{
				std::cout << "\n\t\tclient_max_body_size: ";
				std::cout << servers.at(i).getLocation(j).getMaxBodySize();
			}
			std::map<int, std::string> errorPages = servers.at(i).getLocation(j).getErrorPages();
			if (!errorPages.empty())
			{
				std::cout << "\n\t\terror_page: ";
				for (std::map<int, std::string>::iterator it= errorPages.begin(); it!=errorPages.end(); ++it)
					std::cout << "\n\t\t\t" << it->first << " => " << it->second;
			}
			if (servers.at(i).getLocation(j).getAllowed().size() != 0)
			{
				std::cout << "\n\t\tallowed:";
				for (size_t k = 0; k < servers.at(i).getLocation(j).getAllowed().size(); k++)
				{
					std::cout << " [" << servers.at(i).getLocation(j).getAllow(k) << "]";
				}
			}
			if (servers.at(i).getLocation(j).getDenied().size() != 0)
			{
				std::cout << "\n\t\tdenied:";
				for (size_t k = 0; k < servers.at(i).getLocation(j).getDenied().size(); k++)
				{
					std::cout << " [" << servers.at(i).getLocation(j).getDeny(k) << "]";
				}
			}
			if (servers.at(i).getLocation(j).getReturn().first)
			{
				std::cout << "\n\t\treturn: [" << servers.at(i).getLocation(j).getReturn().first << "]";
				if (servers.at(i).getLocation(j).getReturn().second != "")
					std::cout << " [" << servers.at(i).getLocation(j).getReturn().second << "]";
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
	printServers(servers); // turn on to see parsed configuration settings

	std::cout << "Hello World!" << std::endl;
	//turn the try block of webserv off to test the parsing with make test
	try {
		Webserver	webserv(servers);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}