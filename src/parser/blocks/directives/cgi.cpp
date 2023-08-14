#include "parse.hpp"

t_values	parseCGI(std::string line, t_values values)
{
	std::string reason = "needs one argument: CGI <path>;";

	line = protectedSubstr(line, 3);
	line = ltrim(line);
	checkEmptyString(line, "CGI", reason);
	checkOneArgumentOnly(line, "CGI");
	checkStartingSlash(line, "CGI");
	line = protectedSubstr(line, 1, line.size() - 1);
	checkNoEndingSlash(line, "CGI");
	values.CGI = line;
	return (values);
}
