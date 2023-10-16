#include "parse.hpp"

t_values	parseIndex(std::string line, t_values values)
{
	std::string reason = "needs at least one argument: index <file>";

	line = protectedSubstr(line, 5);
	line = ltrim(line);
	checkEmptyString(line, "index", reason);
	checkNotPreviousDirectory(line, "index");
	while (firstWhitespace(line) != line.size() && firstWhitespace(line) != 0)
	{
		std::string newIndex = protectedSubstr(line, 0, firstWhitespace(line));
		checkHasDot(newIndex, "index");
		if (newIndex.find("/") != 0)
			values.indexes.push_back("/" + newIndex);
		else
			values.indexes.push_back(newIndex);
		line = protectedSubstr(line, firstWhitespace(line) + 1);
		line = ltrim(line);
	}
	if (line != "")
	{
		checkHasDot(line, "index");
		if (line.find("/") != 0)
			values.indexes.push_back("/" + line);
		else
			values.indexes.push_back(line);
	}
	return (values);
}
