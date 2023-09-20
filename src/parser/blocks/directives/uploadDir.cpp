#include "parse.hpp"
#include <iostream>
#include <string>

t_values	parseUploadDir(std::string line, t_values values)
{
	std::string reason = "needs one argument: upload_dir <path>;";

	line = protectedSubstr(line, 10);
	line = ltrim(line);
	checkEmptyString(line, "upload_dir", reason);
	checkOneArgumentOnly(line, "upload_dir");
	checkNotPreviousDirectory(line, "upload_dir");
	checkStartingSlash(line, "upload_dir");
	line = protectedSubstr(line, 1, line.size() - 1);
	checkNoEndingSlash(line, "upload_dir");
	values.uploadDir = line;
	return (values);
}
