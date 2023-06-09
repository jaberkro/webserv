#ifndef PARSE_HPP
# define PARSE_HPP
# include "Config.hpp"

//http parsing
void		parseHTTP(Config &config, std::fstream &file);

//utils
std::string	ltrim(std::string s);
int			getValidLine(std::fstream &file, std::string &line);

#endif
