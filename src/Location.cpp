#include "Location.hpp"

Location::Location() {}

Location::~Location() {}

Location::Location(Location const & loc) : _match (loc.getMatch()), \
_modifier (loc.getModifier()), _root (loc.getRoot()), \
_index (loc.getIndex()), _errorPages (loc.getErrorPages()) {}

Location &	Location::operator=(Location const & loc)
{
	this->_match = loc.getMatch();
	this->_modifier = loc.getModifier();
	this->_root = loc.getRoot();
	this->_index = loc.getIndex();
	this->_errorPages = loc.getErrorPages();
	return (*this);
}

void	Location::setMatch(std::string match)
{
	this->_match = match;
}

void	Location::setModifier(std::string modifier)
{
	this->_modifier = modifier;
}

std::string const &	Location::getRoot(void) const
{
	return (this->_root);
}

void	Location::setRoot(std::string root)
{
	this->_root = root;
}

std::string const &	Location::getIndex(void) const
{
	return (this->_index);
}

void	Location::setIndex(std::string index)
{
	this->_index = index;
}

void	Location::addErrorPage(int key, std::string value)
{
	this->_errorPages.push_back(std::pair<int,std::string>(key, value));
}

std::string const &	Location::getMatch(void) const
{
	return(this->_match);
}

std::string const &	Location::getModifier(void) const
{
	return(this->_modifier);
}


std::vector<std::pair<int,std::string>> const &	Location::getErrorPages(void) const
{
	return(this->_errorPages);
}


