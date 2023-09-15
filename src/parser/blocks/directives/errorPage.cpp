#include "parse.hpp"
#include <string>

t_values	fillDefaultErrorPages(t_values values) // JMA: should this be removed?
{
	values.errorPages[404] = "/404.html";
	values.errorPages[408] = "/408.html";

	return (values);
}

/**
 * @brief parse an error_page directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed error_page.
 */
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
	// values.errorPages.clear(); //use if error_page should overwrite itself
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
