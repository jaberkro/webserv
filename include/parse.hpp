#ifndef PARSE_HPP
# define PARSE_HPP
# include "Server.hpp"
# include "Location.hpp"
# include <map>

typedef struct s_values
{
	std::string					root = "";
	std::vector<std::string>	indexes;
	bool						autoindex = false;
	unsigned int				maxBodySize = 1000000;
	std::map<int, std::string>	errorPages;
	std::vector<std::string>	allowed;
	std::vector<std::string>	denied;
	int							returnCode = 0;
	std::string					returnText = "";
}	t_values;

//utils
std::string protectedSubstr(std::string s, size_t start);
std::string protectedSubstr(std::string s, size_t start, size_t size);

std::string	ltrim(std::string s);
std::string	rtrim(std::string s);
int			getValidLine(std::fstream &file, std::string &line);
size_t		findFirstWhitespace(std::string line);

bool		allDigits(std::string s);

//parsing
void		parseHTTP(std::vector<Server> &servers, std::fstream &file, t_values values);
Server		parseServer(std::fstream &file, t_values values);
Location 	parseLocation(std::fstream &file, std::string line, t_values values);

std::pair<std::string, unsigned short> parseListen(std::string line);
void 		parseServerNames(Server &server, std::string &line);

t_values	parseInheritanceDirective(int directive, std::string line, t_values values);
int			hasInheritanceDirective(std::string line);
t_values	parseLocationDirective(int directive, std::string line, t_values values);
int			hasLocationDirective(std::string line);

t_values	parseRoot(std::string line, t_values values);
t_values	parseIndex(std::string line, t_values values);
t_values	parseAutoindex(std::string line, t_values values);
t_values	parseMaxBodySize(std::string line, t_values values);
t_values	parseErrorPage(std::string line, t_values values);
t_values	parseAllow(std::string line, t_values values);
t_values	parseDeny(std::string line, t_values values);
t_values	parseReturn(std::string line, t_values values);

#endif
