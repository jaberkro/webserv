#include "Location.hpp"

void    Location::setModifier(std::string modifier)
{
	this->_locationModifier = modifier;
}

const std::string    Location::getModifier(void) const
{
	return (this->_locationModifier);
}

void    Location::setMatch(std::string match)
{
	this->_locationMatch = match;
}

const std::string    Location::getMatch(void) const
{
	return (this->_locationMatch);
}

void	Location::setRoot(std::string root)
{
	this->_root = root;
}

std::string	Location::getRoot(void) const
{
	return (this->_root);
}

void	Location::setIndexes(std::vector<std::string> indexes)
{
	this->_indexes = indexes;
}

std::vector<std::string> Location::getIndexes(void) const
{
	return (this->_indexes);
}

std::string	Location::getIndex(size_t i) const
{
	return (this->_indexes.at(i));
}


void	Location::setAutoindex(bool autoindex)
{
	this->_autoindex = autoindex;
}

bool	Location::getAutoindex(void) const
{
	return (this->_autoindex);
}

void	Location::setMaxBodySize(unsigned int maxBodySize)
{
	this->_maxBodySize = maxBodySize;
}

unsigned int	Location::getMaxBodySize(void) const
{
	return (this->_maxBodySize);
}

void	Location::setErrorPages(std::map<int, std::string> errorPages)
{
	this->_errorPages = errorPages;
}


// #include <iostream>	//delete
std::map<int, std::string> const &	Location::getErrorPages(void) const
{
	// if (this->_errorPages.empty())
	// 	std::cout << "Error pages empty." << std::endl;
	// else
	// 	std::cout << "Error pages not empty." << std::endl;
	return (this->_errorPages);
}

void	Location::setAllowed(std::vector<std::string> allowed)
{
	this->_allowed = allowed;
}

std::vector<std::string> Location::getAllowed(void) const
{
	return (this->_allowed);
}

std::string	Location::getAllow(size_t i) const
{
	return (this->_allowed.at(i));
}

void	Location::setDenied(std::vector<std::string> denied)
{
	this->_denied = denied;
}

std::vector<std::string> Location::getDenied(void) const
{
	return (this->_denied);
}

std::string	Location::getDeny(size_t i) const
{
	return (this->_denied.at(i));
}