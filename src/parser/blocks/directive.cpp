#include "parse.hpp"

t_values	parseDirective(int directive, std::string line, t_values values)
{
	switch(directive)
	{
		case 0:
			values = parseRoot(line, values);
			break;
		case 1:
			values = parseIndex(line, values);
			break;
		case 2:
			values = parseAutoindex(line, values);
			break;
		case 3:
			values = parseMaxBodySize(line, values);
			break;
		case 4:
			values = parseErrorPage(line, values);
			break;
		case 5:
			values = parseReturn(line, values);
			break;
		case 6:
			values = parseUploadDir(line, values);
			break;
	}
	return (values);
}

int	hasDirective(std::string line)
{
	std::string directives[] = {"root", "index", "autoindex", \
		"client_max_body_size", "error_page", "return", "upload_dir"};

	int i = 0;

	while (i < 7)
	{
		if (line.find(directives[i]) == 0)
			return (i);
		i++;
	}
	return (-1);
}

t_values	parseLocDirective(int directive, std::string line, t_values values)
{
	switch(directive)
	{
		case 0:
			values = parseAllow(line, values);
			break;
		case 1:
			values = parseDeny(line, values);
			break;
		case 2:
			values = parseCGI(line, values);
			break;
	}
	return (values);
}

int	hasLocDirective(std::string line)
{
	std::string directives[] = {"allow", "deny", "CGI"};

	int i = 0;

	while (i < 3)
	{
		if (line.find(directives[i]) == 0)
			return (i);
		i++;
	}
	return (-1);
}
