#include "Location.hpp"

Location::Location()
{

}

Location::~Location()
{

}

Location::Location(Location &)
{

}

Location &	Location::operator=(Location &)
{
	return (*this);
}

void	Location::setLocationMatch(std::string str)
{
	this->_locationMatch = str;
}

void	Location::setLocationModifier(std::string str)
{
	this->_locationModifier = str;
}

void	Location::addDirective(std::string key, std::string value)
{
	this->_locationDirectives.push_back(std::pair<std::string,std::string>(key, value));
}

void	Location::addErrorPage(int key, std::string value)
{
	this->_errorPages.push_back(std::pair<int,std::string>(key, value));
}

