#include <string>
#include "Request.hpp"
#include <unistd.h>

/**
 * @brief removes the trailing spaces from the beginning and end of a string
 * 
 * @param line string, from which trailing spaces should be removed
 */
void	removeTrailingSpaces(std::string &line)
{
	line.erase(0, line.find_first_not_of(SPACES));
	line.erase(line.find_last_not_of(SPACES) + 1, std::string::npos);
}

/**
 * @brief extracts and returns the string preceding the first ':' character in 
 * the string given as an argument
 * 
 * @param line the string from which the string preceding the first ':' is to be
 * extracted
 * @return std::string - the extracted string
 */
std::string	extractKey(std::string line)
{
	size_t	colon = line.find_first_of(":");
	
	return (line.substr(0, colon));
}


/**
 * @brief extracts and returns the string immediately following the first ':' 
 * character in the string given as an argument. The function removes trailing
 * spaces.
 * 
 * @param line the string from which the string following the first ':' is to be
 * extracted
 * @return std::string - the extracted string
 */
std::string	extractValue(std::string line)
{
	size_t		colon = line.find_first_of(":");
	std::string	value;
	
	value = colon == std::string::npos ? "80" : line.substr(colon + 1, std::string::npos);
	removeTrailingSpaces(value);
	return (value);
	// RETURNS 80 IF EMPTY -> COULD BE MOVED TO A SEPARATE FUNCTION
}


/**
 * @brief extracts a substring from beginning of a buffer until the following
 * new line (at the index of nlPos), deletes it from the buffer and moves it 
 * into the line. The new line character is discarded in the process.
 * 
 * @param buffer source of the to be extracted string; the string will be removed
 * from the buffer
 * @param line destination in which the extracted string is placed
 * @param nlPos denotes the position of the closest new line character
 */
void	extractStr(std::string &buffer, std::string &line, size_t nlPos)
{
	line = buffer.substr(0, nlPos - 1);
	buffer.erase(0, nlPos + 1);
}

/**
 * @brief counts the overlapping elements in hostnames starting with a leading
 * asterisk
 * 
 * @param hostSplit hostname in the request, split by element ('.' or string)
 * @param nameSplit server_name, split by element ('.' or string)
 * @return size_t index of the server with longest overlap
 */
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


/**
 * @brief counts the overlapping elements in hostnames starting with a trailing
 * asterisk
 * 
 * @param hostSplit hostname in the request, split by element ('.' or string)
 * @param nameSplit server_name, split by element ('.' or string)
 * @return size_t index of the server with longest overlap
 */
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

/**
 * @brief transforms a string into lowercase
 * 
 * @param str reference to a string to be transformed into lowercase
 */
void	makeLowercase(std::string & str)
{
	for (size_t idx = 0; idx < str.length(); idx++)
		str[idx] = tolower(str[idx]);
}

/**
 * @brief splits a string (server name) into elements of '.' and strings of other
 * characters
 * 
 * @param name domain name to be split
 * @param chunks vector in which the elements are saved
 */

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