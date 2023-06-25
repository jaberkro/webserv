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
