#include "Server.hpp"
#include <iostream>

static void printReturn(Location location)
{
	if (location.getReturnCode() > 0)
	{
		std::cout << "\n\t\treturn: [" << location.getReturnCode() << "]";
		if (location.getReturnLink() != "")
			std::cout << " [" << location.getReturnLink() << "]";
		else if (location.getReturnLink() != "")
			std::cout << " [" << location.getReturnLink() << "]";
	}
}

static void printAllowedDenied(Location location)
{
	if (location.getAllowed().size() != 0)
	{
		std::cout << "\n\t\tallowed:";
		for (size_t k = 0; k < location.getAllowed().size(); k++)
			std::cout << " [" << location.getAllow(k) << "]";
	}
	if (location.getDenied().size() != 0)
	{
		std::cout << "\n\t\tdenied:";
		for (size_t k = 0; k < location.getDenied().size(); k++)
			std::cout << " [" << location.getDeny(k) << "]";
	}
}

static void	printErrorPages(std::map<int, std::string> errorPages)
{
	if (!errorPages.empty())
	{
		std::cout << "\n\terror_page: ";
		std::cout << "size: " << errorPages.size();
		for (std::map<int, std::string>::iterator sit= errorPages.begin(); \
		sit!=errorPages.end(); ++sit)
		{
			std::cout << "\n\t\t[" << sit->first << "] => [";
			std::cout << sit->second << "]";
		}
	}
}

void	printLocation(Location location)
{
	std::cout << "\n\tlocation ";
	if (location.getModifier() != "(none)")
		std::cout << location.getModifier() << " ";
	std::cout << location.getMatch() << ":";
	std::cout << "\n\t\troot: [" << location.getRoot() << "]";
	if (location.getIndexes().size())
	{
		std::cout << "\n\t\tindex:";
		for (size_t k = 0; k < location.getIndexes().size(); k++)
			std::cout << " [" << location.getIndex(k) << "]";
	}
	if (location.getAutoindex() == true)
		std::cout << "\n\t\tautoindex: on";
	if (location.getMaxBodySize() != 1000000)
	{
		std::cout << "\n\t\tclient_max_body_size: [";
		std::cout << location.getMaxBodySize() << "]";
	}
	printErrorPages(location.getErrorPages());
	printAllowedDenied(location);
	printReturn(location);
	if (location.getUploadDir().size() != 0)
		std::cout << "\n\t\tupload_dir: [" << location.getUploadDir() << "]";
}

void	printServers(std::vector<Server> servers)
{
	std::cout << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "server " << i << ":";
		for (size_t j = 0; j < servers.at(i).getListens().size(); j++)
		{
			std::cout << "\n\thost: [" << servers.at(i).getHost(j) << "]\t";
			std::cout << "port: [" << servers.at(i).getPort(j) << "]";
		}
		if (servers.at(i).getServerNames().size() > 0)
			std::cout << "\n\tserver_name:";
		for (size_t j = 0; j < servers.at(i).getServerNames().size(); j++)
			std::cout << " [" << servers.at(i).getServerName(j) << "]";
		printErrorPages(servers.at(i).getErrorPages());
		for (size_t j = 0; j < servers.at(i).getLocations().size(); j++)
		{
			printLocation(servers.at(i).getLocation(j));
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
