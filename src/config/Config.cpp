#include "Config.hpp"
#include <iostream>

Config::Config()
{
	this->_servers.reserve(10);
	// std::cout << "Default constructor called on Config" << std::endl;
}

Config::Config(const Config &src)
{
	*this = src;
	// std::cout << "Copy constructor called on Config" << std::endl;
}

Config& Config::operator=(const Config &src)
{
	this->_servers = src._servers;
	// std::cout << "Copy assignment operator called on Config" << std::endl;
	return (*this);
}

Config::~Config(void)
{
	// std::cout << "Destructor called on Config" << std::endl;
}

void Config::addServer(Server &server)
{
	this->_servers.push_back(server);
}

const Server& Config::getServer(int i) const
{
	return (this->_servers.at(i));
}

const std::vector<Server>& Config::getServers() const
{
	return (this->_servers);
}
