#include <string>
#include <iostream>

void notImplementedError(std::string line, std::string here, std::string block)
{
	std::cerr << "Error: [" << line << "]: not implemented in " << here;
	std::cerr << " block: can only be inside " << block << std::endl;
	exit(EXIT_FAILURE);
}

void notClosedError(std::string block)
{
	std::cerr << "Error: " << block << " not closed before ";
	std::cerr << "end of file: } missing" << std::endl;
	exit(EXIT_FAILURE);
}

void notRecognizedError(std::string line, std::string here)
{
	std::cerr << "Error: can't parse " << here << " near ";
	std::cerr << "[" << line << "]: not recognized" << std::endl;
	exit(EXIT_FAILURE);
}

void noServerError(void)
{
	std::cerr << "Error: can't parse http block without server block inside ";
	std::cerr << "of it: \nhttp {\n\tserver {\n\n\t}\n}" << std::endl;
	exit(EXIT_FAILURE);
}

void portError(std::string notPort)
{
	std::cerr << "Error: incorrect port in configuration file: [" << notPort;
	std::cerr << "]: port must be a number in range [0, 65535]" << std::endl;
	exit(EXIT_FAILURE);
}

void hostError(std::string notHost)
{
	std::cerr << "Error: can't parse listen directive: invalid host: [";
	std::cerr << notHost << "]: invalid IP address" << std::endl;
	exit(EXIT_FAILURE);
}

void methodError(std::string line, std::string directive, std::string allowed)
{
	std::cerr << "Error: can't parse " << directive << ": invalid method: [";
	std::cerr << line << "]: allowed methods are: " << allowed << std::endl;
	exit(EXIT_FAILURE);
}

void tooBigError(std::string line, std::string directive, std::string max)
{
	std::cerr << "Error: can't parse " << directive << " directive: [" << line;
	std::cerr << "]: input number too big: max " << max << std::endl;
	exit(EXIT_FAILURE);
}

void nanError(std::string line, std::string directive)
{
	std::cerr << "Error: can't parse " << directive << " directive: ";
	std::cerr << "[" << line << "]: not a number" << std::endl;
	exit(EXIT_FAILURE);
}

void rootError(std::string line, std::string reason)
{
	std::cerr << "Error: can't parse root directive: ";
	std::cerr << "[" << line << "]: " << reason << std::endl;
	exit(EXIT_FAILURE);
}
