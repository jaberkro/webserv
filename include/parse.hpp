#ifndef PARSE_HPP
# define PARSE_HPP
# include "Config.hpp"
# include "Location.hpp"

typedef struct s_values
{
	std::string					root;
	std::vector<std::string>	indexes;
	bool						autoindex;
	unsigned int				maxBodySize = 1000000;
}	t_values;

//utils
std::string protectedSubstr(std::string s, size_t start);
std::string protectedSubstr(std::string s, size_t start, size_t size);

std::string	ltrim(std::string s);
std::string	rtrim(std::string s);
int			getValidLine(std::fstream &file, std::string &line);
size_t		findFirstWhitespace(std::string line);

bool		allDigits(std::string s);

//http parsing
void		parseHTTP(Config &config, std::fstream &file, t_values values);
Server 		parseServer(std::fstream &file, t_values values);
Location 	parseLocation(std::fstream &file, std::string line, t_values values);

std::pair<std::string, unsigned short> parseListen(std::string line);
void 				parseServerNames(Server &server, std::string &line);

t_values			parseRoot(std::string line, t_values values);
t_values			parseIndex(std::string &line, t_values values); //line copy or reference? inconsistent
t_values			parseAutoindex(std::string &line, t_values values);
t_values			parseMaxBodySize(std::string line, t_values values);

t_values	parseInheritanceDirective(int directive, std::string line, t_values values);
int			hasInheritanceDirective(std::string line);

#endif
