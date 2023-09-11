#include "parse.hpp"
#include <string>

/**
 * @brief checks if argument is one quote startign and ending with '"'
 * 
 * @param text the text to check
 * @return true if the text is one quote
 * @return false if the text is not one quote
 */
static bool quotedArgument(std::string text)
{
	if (text.size() < 2 || text.at(0) != '"' || text.back() != '"')
	{
		return (false);
	}
	for (size_t i = 1; i < text.size() - 2; i++)
	{
		if (text.at(i) == '"')
			return (false);
	}
	return (true);
}

/**
 * @brief parse a return directive and store it in values
 * 
 * @param line the line to parse
 * @param values the values to be updated
 * @return t_values the updated values
 */
t_values	parseReturn(std::string line, t_values values)
{
	std::string	reason = "needs at least one argument: return <code> [text];";
	std::string	code;
	size_t		firstSpace;

	line = protectedSubstr(line, 6);
	line = ltrim(line);
	checkEmptyString(line, "return", reason);
	firstSpace = firstWhitespace(line);
	code = protectedSubstr(line, 0, firstSpace);
	values.returnCode = parseErrorCode(code, "return");
	line = protectedSubstr(line, firstSpace, line.size() - firstSpace);
	line = ltrim(line);
	if (!quotedArgument(line))
	{
		checkOneArgumentOnly(line, "return text parameter");
		checkNotPreviousDirectory(line, "return");
	}
	else
		line = protectedSubstr(line, 1, line.size() - 2);
	values.returnText = line;
	return (values);
}

// CHECK IF IT IS LINK: LINK HAS '.' OR '/' OR ':'
// FILL CORRECT PAIR OF RETURN 
