#include "Location.hpp"

Location::Location()
{
	// std::cout << "Default constructor called on Location" << std::endl;
}

Location::Location(const Location &src)
{
	*this = src;
	// std::cout << "Copy constructor called on Location" << std::endl;
}

Location& Location::operator=(const Location &src)
{
	this->_allowed = src._allowed;
	this->_autoindex = src._autoindex;
	this->_denied = src._denied;
	this->_errorPages = src._errorPages;
	this->_indexes = src._indexes;
	this->_locationDirectives = src._locationDirectives;
	this->_locationMatch = src._locationMatch;
	this->_locationModifier = src._locationModifier;
	this->_maxBodySize = src._maxBodySize;
	this->_return = src._return;
	this->_root = src._root;
	this->_uploadDir = src._uploadDir;
	// std::cout << "Copy assignment operator called on Location" << std::endl;
	return (*this);
}

Location::~Location(void)
{
	// std::cout << "Destructor called on Location" << std::endl;
}


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


std::map<int, std::string> const &	Location::getErrorPages(void) const
{
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

void    Location::setReturn(int code, std::string text)
{
	this->_return = make_pair(code, text);
}

const std::pair<int, std::string>    Location::getReturn(void) const
{
	return (this->_return);
}

void	Location::setUploadDir(std::string uploadDir)
{
	this->_uploadDir = uploadDir;
}

std::string	Location::getUploadDir(void) const
{
	return (this->_uploadDir);
}
