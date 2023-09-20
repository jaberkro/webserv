#include "Server.hpp"
#include <iostream>

Server::Server(){}

Server::Server(const Server &src)
{
	*this = src;
}

Server& Server::operator=(const Server &src)
{
	this->_listens = src._listens;
	this->_serverNames = src._serverNames;
	this->_errorPages = src._errorPages;
	this->_locations = src._locations;
	return (*this);
}

Server::~Server(void){}

void Server::addListen(std::pair<std::string, unsigned short> listen)
{
	this->_listens.push_back(listen);
}

void Server::addServerName(std::string name)
{
	this->_serverNames.push_back(name);
}

void Server::addLocation(Location location)
{
	this->_locations.push_back(location);
}

const std::vector<std::pair<std::string, unsigned short>> &	\
Server::getListens(void) const
{
	return (this->_listens);
}

const std::string Server::getHost(int i) const
{
	return (std::get<0>(this->_listens.at(i)));
}

unsigned short Server::getPort(int i) const
{
	return (std::get<1>(this->_listens.at(i)));
}

const std::vector<std::string> Server::getServerNames(void) const
{
	return (this->_serverNames);
}

const std::string Server::getServerName(int i) const
{
	if (this->_serverNames.size() > 0)
		return (this->_serverNames.at(i));
	else
		return ("< Server has no name >");
}

const std::vector<Location> & Server::getLocations(void) const
{
	return (this->_locations);
}

const Location & Server::getLocation(int i) const
{
	return (this->_locations.at(i));
}

void	Server::setErrorPages(std::map<int, std::string> errorPages)
{
	this->_errorPages = errorPages;
}

std::map<int, std::string> const &	Server::getErrorPages(void) const
{
	return (this->_errorPages);
}
