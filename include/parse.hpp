#ifndef PARSE_HPP
# define PARSE_HPP
# include "Config.hpp"
# include "Location.hpp"

//utils
std::string protectedSubstr(std::string s, size_t start);
std::string protectedSubstr(std::string s, size_t start, size_t size);

std::string	ltrim(std::string s);
std::string	rtrim(std::string s);
int			getValidLine(std::fstream &file, std::string &line);
size_t		findFirstWhitespace(std::string line);

bool		allDigits(std::string s);

//http parsing
void		parseHTTP(Config &config, std::fstream &file);
Server 		parseServer(std::fstream &file);
std::pair<std::string, unsigned short> parseListen(std::string line);
void 		parseServerNames(Server &server, std::string &line);
Location 	parseLocation(std::fstream &file, std::string line);
bool		parseAutoindex(std::string &line);




#endif
