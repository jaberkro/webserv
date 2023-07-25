#include <string>
#include <iostream>

void notImplementedError(std::string line, std::string here, std::string block)
{
	std::cout << "Error: [" << line << "]: not implemented in " << here;
	std::cout << " block: can only be inside " << block << std::endl;
	exit(EXIT_FAILURE);
}

void notClosedError(std::string block)
{
	std::cout << "Error: " << block << " not closed before ";
	std::cout << "end of file: } missing" << std::endl;
	exit(EXIT_FAILURE);
}

void notRecognizedError(std::string line, std::string here)
{
	std::cout << "Error: can't parse " << here << " block near ";
	std::cout << "[" << line << "]: not recognized" << std::endl;
	exit(EXIT_FAILURE);
}

void noServerError(void)
{
	std::cout << "Error: can't parse http block without server block inside ";
	std::cout << "of it: \nhttp {\n\tserver {\n\n\t}\n}" << std::endl;
	exit(EXIT_FAILURE);
}

void portError(std::string notPort)
{
	std::cout << "Error: incorrect port in configuration file: [" << notPort;
	std::cout << "]: port must be a number in range [0, 65535]" << std::endl;
	exit(EXIT_FAILURE);
}

void hostError(std::string notHost)
{
	std::cout << "Error: can't parse listen directive: invalid host: " << \
		notHost << ": invalid IP address" << std::endl;
	exit(EXIT_FAILURE);
}

void methodError(std::string line, std::string directive, std::string allowed)
{
	std::cout << "Error: can't parse " << directive << ": invalid method: [";
	std::cout << line << "]. Allowed methods are: " << allowed << std::endl;
	exit(EXIT_FAILURE);
}

void tooBigError(std::string line, std::string directive, std::string max)
{
	std::cout << "Error: can't parse " << directive << " directive: [" << line;
	std::cout << "]: input number too big: max " << max << std::endl;
	exit(EXIT_FAILURE);
}

void nanError(std::string line, std::string directive)
{
	std::cout << "Error: can't parse " << directive << " directive: ";
	std::cout << "[" << line << "]: not a number" << std::endl;
	exit(EXIT_FAILURE);
}
