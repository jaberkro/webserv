// #include "webserv.hpp"
#include "parse.hpp"
#include <iostream>
#include "Webserver.hpp"
// #include <map>

void	func_atexit(void)
{
	system("leaks webserv");
}

int	main(int argc, char **argv)
{
	std::vector<Server> servers;
	char	*defaultFile = strdup("config/new.conf");

	if (argc > 2)
	{
		std::cerr << "usage: ./webserv [configuration file]" << std::endl;
		return (1);
	}
	else if (argc == 1)
		parse(servers, defaultFile);
	else
		parse(servers, argv[1]);
	// try{
	printServers(servers); // turn on to see parsed configuration settings
	// } 
	// catch (const std::exception& e)
	// {
		// std::cerr << "Gaat dit fout?" << '\n';
		//std::cerr << e.what() << '\n';
	// }
	std::cout << "Hello World!" << std::endl;
	//turn the try block of webserv off to test the parsing with make test
	try {
		Webserver	webserv(servers);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	// atexit(func_atexit);
	return (0);
}
