#include <iostream>
#include "webserver.hpp"

int	main(void)
{
	std::cout << "Hello World!" << std::endl;
	Webserver	webserv;
	webserv.start();//servers);
	return (0);
}
