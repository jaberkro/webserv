#ifndef PARSE_HPP
# define PARSE_HPP
# include "Config.hpp"
# include "Location.hpp"

//utils
std::string	ltrim(std::string s);
int			getValidLine(std::fstream &file, std::string &line);
size_t		findFirstWhitespace(std::string line);

//http parsing
void		parseHTTP(Config &config, std::fstream &file);
Server 		parseServer(std::fstream &file);
std::pair<std::string, unsigned short> parseListen(std::string line);
void 		parseServerNames(Server &server, std::string &line);
Location 	parseLocation(std::fstream &file, std::string line);




#endif
