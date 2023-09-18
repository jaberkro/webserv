#include "responseCodes.hpp"
#include <fstream>

/**
 * @brief finds out the size of a file
 * 
 * @param filePath path to the file, the size of which is to be determined
 * @return size_t indicating the size of the file (in bytes)
 */
size_t	getFileSize(std::string filePath)
{
	std::ifstream	file;
	size_t			len;

	file.open(filePath, std::ifstream::binary | std::ifstream::ate);	
	if (file.is_open())
	{
		len = file.tellg();
		file.clear();
		file.close();
		return (len);
	}
	return (0);
}

bool	isContentAllowed(int statusCode)
{
	if (statusCode >= OK && statusCode != DELETED && statusCode != NOT_MODIFIED)
		return (true);
	return (false);
}

bool	isRequestedByCurl(std::string userAgent)
{
	if (userAgent.find("curl") == 0)
		return (true);
	return (false);
}
