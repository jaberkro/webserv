#include "Location.hpp"

Location::Location()
{
	this->_autoindex = false;
	this->_locationMatch = "";
	this->_locationModifier = "";
	this->_maxBodySize = 1000000;
	this->_returnCode = 0;
	this->_returnLink = "";
	this->_returnMessage = "";
	this->_root = "";
	this->_uploadDir = "";
	this->_cgiScriptName = "";
}

Location::Location(const Location &src)
{
	*this = src;
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
	this->_returnCode = src._returnCode;
	this->_returnLink = src._returnLink;
	this->_returnMessage = src._returnMessage;
	this->_root = src._root;
	this->_uploadDir = src._uploadDir;
	this->_cgiScriptName = src._cgiScriptName;
	return (*this);
}

Location::~Location(){}

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

void	Location::setMaxBodySize(unsigned long long maxBodySize)
{
	this->_maxBodySize = maxBodySize;
}

unsigned long long	Location::getMaxBodySize(void) const
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
	this->_returnCode = code;
	if (text.find_first_of(".:/") != std::string::npos)
		this->_returnLink = text;
	else
		this->_returnMessage = text;
}

int    Location::getReturnCode(void) const
{
	return (this->_returnCode);
}

const std::string    Location::getReturnLink(void) const
{
	return (this->_returnLink);
}

const std::string    Location::getReturnMessage(void) const
{
	return (this->_returnMessage);
}

void	Location::setUploadDir(std::string uploadDir)
{
	this->_uploadDir = uploadDir;
}

std::string	Location::getUploadDir(void) const
{
	return (this->_uploadDir);
}

void	Location::setCgiScriptName(std::string cgiScriptName)
{
	this->_cgiScriptName = cgiScriptName;
}

std::string	Location::getCgiScriptName(void) const
{
	return (this->_cgiScriptName);
}
