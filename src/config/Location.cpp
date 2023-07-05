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
