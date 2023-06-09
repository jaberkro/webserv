#include "Server.hpp"
#include <iostream>

Server::Server()
{
	this->_ports.push_back(80);
	this->_hosts.push_back("localhost");
	this->_serverNames.push_back("");
	// std::cout << "Default constructor called on Server" << std::endl;
}

Server::Server(std::vector<unsigned short> ports, std::vector<std::string> hosts, std::vector<std::string> serverNames)
{
	this->_ports = ports;
	this->_hosts = hosts;
	this->_serverNames = serverNames;
	// std::cout << "Parametric constructor called on Server" << std::endl;
}

Server::Server(const Server &src)
{
	*this = src;
	// std::cout << "Copy constructor called on Server" << std::endl;
}

Server& Server::operator=(const Server &src)
{
	this->_ports = src._ports;
	this->_hosts = src._hosts;
	this->_serverNames = src._serverNames;
	// std::cout << "Copy assignment operator called on Server" << std::endl;
	return (*this);
}

Server::~Server(void)
{
	// std::cout << "Destructor called on Server" << std::endl;
}

void Server::addPort(unsigned short port)
{
	this->_ports.push_back(port);
}

unsigned short Server::getPort(int i) const
{
	return (this->_ports.at(i));
}

const std::vector<unsigned short> Server::getPorts(void) const
{
	return (this->_ports);
}

const std::string Server::getHost(int i) const
{
	return (this->_hosts.at(i));
}

const std::vector<std::string> Server::getHosts(void) const
{
	return (this->_hosts);
}

const std::string Server::getServerName(int i) const
{
	return (this->_serverNames.at(i));
}

const std::vector<std::string> Server::getServerNames(void) const
{
	return (this->_serverNames);
}
