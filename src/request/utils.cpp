#include <string>
#include "Request.hpp"
#include <unistd.h>

void	removeTrailingSpaces(std::string &line)
{
	line.erase(0, line.find_first_not_of(SPACES));
	line.erase(line.find_last_not_of(SPACES) + 1, std::string::npos);
}

std::string	extractKey(std::string line)
{
	size_t	colon = line.find_first_of(":");
	
	return (line.substr(0, colon));
}


std::string	extractValue(std::string line)
{
	size_t		colon = line.find_first_of(":");
	std::string	value;
	
	value = colon == std::string::npos ? "" : line.substr(colon + 1, std::string::npos);
	removeTrailingSpaces(value);
	return (value);
}

void	extractStr(std::string &buffer, std::string &line, size_t nlPos)
{
	line = buffer.substr(0, nlPos - 1);
	buffer.erase(0, nlPos + 1);
}

size_t	countOverlapLeading(std::vector<std::string> & hostSplit, \
std::vector<std::string> & nameSplit)
{
	size_t	revIdxN = nameSplit.size() - 1;
	size_t	revIdxH = hostSplit.size() - 1;

	for (size_t idx = 0; idx < hostSplit.size(); idx++)
	{
		if (nameSplit[revIdxN - idx] == "*")
			return (idx);
		if (nameSplit[revIdxN - idx] != hostSplit[revIdxH - idx])
			break;
	}
	return (0);
}

size_t	countOverlapTrailing(std::vector<std::string> & hostSplit, \
std::vector<std::string> & nameSplit)
{
	for (size_t idx = 0; idx < hostSplit.size(); idx++)
	{
		if (nameSplit[idx] == "*")
			return (idx);
		if (nameSplit[idx] != hostSplit[idx])
			break;
	}
	return (0);		
}

void	makeLowercase(std::string & str)
{
	for (size_t idx = 0; idx < str.length(); idx++)
		str[idx] = tolower(str[idx]);
}

std::vector<std::string>	splitServerName(std::string const & name)
{
	size_t	begin = 0;
	size_t	end;
	std::vector<std::string>	chunks;

	while (begin < name.length())
	{
		end = name[begin] == '.' ? name.find_first_of('.', begin) + 1 : name.find_first_of('.', begin);
		chunks.push_back(name.substr(begin, end - begin));
		begin = end;
	}
	return (chunks);
}

bool	hasReadPermission(std::string filePath)
{
	if (access(filePath.c_str(), F_OK | R_OK) < 0)
		return (false);
	else
		return (true);
}
