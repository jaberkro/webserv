#include "parse.hpp"
#include <iostream>
#include <string>

static bool allowedDir(std::string line)
{
	if (line == "/www" || line == "/images")
		return (false);
	return (true);
}

/**
 * @brief parse a upload_dir directive
 * 
 * @param line the line to parse
 * @param values the struct to update
 * @return t_values the updated struct containing the parsed upload_dir
 */
t_values	parseUploadDir(std::string line, t_values values)
{
	std::string reason = "needs one argument: upload_dir <path>;";

	line = protectedSubstr(line, 10);
	line = ltrim(line);
	checkEmptyString(line, "upload_dir", reason);
	checkOneArgumentOnly(line, "upload_dir");
	checkStartingSlash(line, "upload_dir");
	line = protectedSubstr(line, 1, line.size() - 1);
	checkNoEndingSlash(line, "upload_dir");
	if (!allowedDir(line))
	{
		std::cerr << "Error: invalid upload_dir: " << line;
		std::cerr << ": already exists" << std::endl;
	}
	values.uploadDir = line;
	return (values);
}

//WHAT IF USER DEFINES WWW AS UPLOAD_DIR???? OR IMAGES???