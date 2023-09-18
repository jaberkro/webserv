#ifndef PARSE_HPP
# define PARSE_HPP
# include "Server.hpp"
# include "Location.hpp"
# include <map>
# include <vector>

typedef struct s_values
{
	std::string					root = "";
	std::vector<std::string>	indexes;
	bool						autoindex = false;
	unsigned long long			maxBodySize = 1000000;
	std::map<int, std::string>	errorPages;
	std::vector<std::string>	allowed;
	std::vector<std::string>	denied;
	int							returnCode = 0;
	std::string					returnText = "";
	std::string					uploadDir = "";
	std::string					CGI = "";
}	t_values;

//parsing
void		parse(std::vector<Server> &servers, char *configFile);
t_values	fillDefaultErrorPages(t_values values);

//parsing blocks
void		parseHTTP(std::vector<Server> &servers, std::fstream &file);
Server		parseServer(std::fstream &file, t_values values);
Location 	parseLocation(std::fstream &file, std::string line, t_values values);
t_values	parseDirective(int directive, std::string line, t_values values);
int			hasDirective(std::string line);
t_values	parseLocDirective(int directive, std::string line, t_values values);
int			hasLocDirective(std::string line);

// parsing directives
std::pair<std::string, unsigned short> parseListen(std::string line);
void 		parseServerNames(Server &server, std::string &line);
t_values	parseRoot(std::string line, t_values values);
t_values	parseIndex(std::string line, t_values values);
t_values	parseAutoindex(std::string line, t_values values);
t_values	parseMaxBodySize(std::string line, t_values values);
t_values	parseErrorPage(std::string line, t_values values);
t_values	parseAllow(std::string line, t_values values);
t_values	parseDeny(std::string line, t_values values);
t_values	parseReturn(std::string line, t_values values);
t_values	parseUploadDir(std::string line, t_values values);
t_values	parseCGI(std::string line, t_values values);

//utils
std::string protectedSubstr(std::string s, size_t start);
std::string protectedSubstr(std::string s, size_t start, size_t size);

std::string	ltrim(std::string s);
std::string	rtrim(std::string s);
size_t		firstWhitespace(std::string line);
int			getValidLine(std::fstream &file, std::string &line);

bool		allDigits(std::string s);
int			parseErrorCode(std::string code, std::string directive);
std::string	convertToLower(std::string str);
void		printServers(std::vector<Server> servers);

//check_exit
void		checkEmptyString(std::string line, std::string directive, std::string why);
void		checkOneArgumentOnly(std::string line, std::string directive);
void		checkStartingSlash(std::string line, std::string directive);
void		checkNoEndingSlash(std::string line, std::string directive);
void		checkHasDot(std::string line, std::string directive);
void		checkNotPreviousDirectory(std::string line, std::string directive);
void		checkMissingHTTP(std::vector<Server> & servers);

//error
void		notImplementedError(std::string line, std::string here, std::string block);
void		notClosedError(std::string);
void 		notRecognizedError(std::string line, std::string here);
void		noLocationError(void);
void		noServerError(void);
void		methodError(std::string line, std::string directive, std::string allowed);
void		portError(std::string notPort);
void		hostError(std::string notHost);
void		rootError(std::string line, std::string reason);
void 		tooBigError(std::string line, std::string directive, std::string max);
void 		nanError(std::string line, std::string directive);

#endif
