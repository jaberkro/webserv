#include "parse.hpp"

t_values	fillDefaultErrorPages(t_values values)
{
	values.errorPages[400] = "/400.html";
	values.errorPages[403] = "/403.html";
	values.errorPages[404] = "/404.html";
	values.errorPages[405] = "/405.html";
	values.errorPages[408] = "/408.html";
	values.errorPages[413] = "/413.html";
	values.errorPages[500] = "/500.html";
	values.errorPages[501] = "/501.html";
	return (values);
}

t_values	parseErrorPage(std::string line, t_values values)
{
	std::string	reason = "needs at least two arguments: error_page";
	std::string	file;
	std::string	code;
	size_t		lastWhitespace;

	line = protectedSubstr(line, 10);
	line = ltrim(line);
	checkEmptyString(line, "error_page", reason + " <code> <file>");
	checkNotPreviousDirectory(line, "error_page");
	lastWhitespace = line.find_last_of(" \t\v\b") + 1;
	file = protectedSubstr(line, lastWhitespace, line.size() - lastWhitespace);
	checkStartingSlash(file, "error_page");
	checkHasDot(file, "error_page");
	line = protectedSubstr(line, 0, line.find_last_of(" \t\v\b") + 1);
	line = rtrim(line);
	while (firstWhitespace(line) != line.size() && firstWhitespace(line) != 0)
	{
		code = protectedSubstr(line, 0, firstWhitespace(line));
		values.errorPages[parseErrorCode(code, "error_page")] = file;
		line = protectedSubstr(line, firstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
		values.errorPages[parseErrorCode(line, "error_page")] = file;
	return (values);
}
