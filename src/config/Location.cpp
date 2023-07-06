#include "Location.hpp"

Location::Location() : \
_match (""), \
_modifier (""), \
_root (""), \
_index (""), \
_autoindex (false), \
_maxBodySize (0) {}

Location::~Location() {}

Location::Location(Location const & loc) : \
_match (loc.getMatch()), \
_modifier (loc.getModifier()), \
_root (loc.getRoot()), \
_errorPages (loc.getErrorPages()),
_indexes (loc.getIndexes()),
_autoindex (loc.getAutoindex()), \
_maxBodySize (loc.getMaxBodySize()) {}

Location &	Location::operator=(Location const & loc)
{
	this->_match = loc.getMatch();
	this->_modifier = loc.getModifier();
	this->_root = loc.getRoot();
	this->_errorPages = loc.getErrorPages();
	this->_indexes = loc.getIndexes();
	this->_autoindex = loc.getAutoindex();
	this->_maxBodySize = loc.getMaxBodySize();
	return (*this);
}

void	Location::setModifier(std::string modifier)
{
	this->_modifier = modifier;
}

std::string const &	Location::getModifier(void) const
{
	return(this->_modifier);
}

void	Location::setMatch(std::string match)
{
	this->_match = match;
}

std::string const &	Location::getMatch(void) const
{
	return(this->_match);
}

void	Location::setRoot(std::string root)
{
	this->_root = root;
}

std::string const &	Location::getRoot(void) const
{
	return (this->_root);
}

void	Location::addIndex(std::string index)
{
	this->_indexes.push_back(index);
}

void	Location::setIndexes(std::vector<std::string> indexes)
{
	this->_indexes = indexes;
}

std::vector<std::string> Location::getIndexes(void) const
{
	return (this->_indexes);
}

// std::string	Location::getIndex(size_t i) const
// {
// 	return (this->_indexes.at(i));
// }

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

void	Location::addErrorPage(int key, std::string value)
{
	this->_errorPages.insert(std::pair<int,std::string>(key, value));
}

std::map<int,std::string> const &	Location::getErrorPages(void) const
{
	return(this->_errorPages);
}


