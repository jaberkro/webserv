#include "Server.hpp"
#include <vector>
#include <string>
#include <map>
#include <iostream>

void	printLocation(Location location)
{
	std::cout << "\n\tlocation ";
	if (location.getModifier() != "(none)")
		std::cout << location.getModifier() << " " << location.getMatch() << ":";
	if (location.getRoot().size())
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
		std::cout << "\n\t\tclient_max_body_size: [" << location.getMaxBodySize() << "]";
	std::map<int, std::string> errorPages = location.getErrorPages();
	if (!errorPages.empty())
	{
		std::cout << "\n\t\terror_page: ";
		for (std::map<int, std::string>::iterator it= errorPages.begin(); it!=errorPages.end(); ++it)
			std::cout << "\n\t\t\t[" << it->first << "] => [" << it->second << "]";
	}
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
	if (location.getReturn().first)
	{
		std::cout << "\n\t\treturn: [" << location.getReturn().first << "]";
		if (location.getReturn().second != "")
			std::cout << " [" << location.getReturn().second << "]";
	}
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
		std::map<int, std::string> errorPages = servers.at(i).getErrorPages();
		if (!errorPages.empty())
		{
			std::cout << "\n\terror_page: ";
			std::cout << "size: " << errorPages.size();
			for (std::map<int, std::string>::iterator sit= errorPages.begin(); sit!=errorPages.end(); ++sit)
				std::cout << "\n\t\t[" << sit->first << "] => [" << sit->second << "]";
		}
		for (size_t j = 0; j < servers.at(i).getLocations().size(); j++)
		{
			printLocation(servers.at(i).getLocation(j));
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
