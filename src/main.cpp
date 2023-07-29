#include "webserv.hpp"
#include "parse.hpp"
#include <iostream>
#include "Webserver.hpp"
// #include <map>

int	main(int argc, char **argv)
{
	std::vector<Server> servers;

	if (argc != 2)
	{
		std::cout << "usage: ./webserv [configuration file]" << std::endl;
		return (1);
	}
	try{
	parse(servers, argv[1]);
	// printServers(servers); // turn on to see parsed configuration settings
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Gaat dit fout?" << '\n';
		//std::cerr << e.what() << '\n';
	}
	std::cout << "Hello World!" << std::endl;
	//turn the try block of webserv off to test the parsing with make test
	try {
		Webserver	webserv(servers);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
