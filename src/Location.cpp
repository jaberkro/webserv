#include "Location.hpp"

Location::Location()
{

}

Location::~Location()
{

}

Location::Location(Location const & loc)
{
	this->_locationMatch = loc.getLocationMatch();
	this->_locationModifier = loc.getLocationModifier();
	this->_locationDirectives = loc.getLocationDirectives();
	this->_errorPages = loc.getErrorPages();
}

Location &	Location::operator=(Location const &)
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

std::string const &	Location::getLocationMatch(void) const
{
	return(this->_locationMatch);
}

std::string const &	Location::getLocationModifier(void) const
{
	return(this->_locationModifier);
}

std::vector<std::pair<std::string,std::string>> const &	Location::getLocationDirectives(void) const
{
	return(this->_locationDirectives);
}

std::vector<std::pair<int,std::string>> const &	Location::getErrorPages(void) const
{
	return(this->_errorPages);
}


