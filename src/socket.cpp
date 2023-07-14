#include "../include/socket.hpp"
#include "../include/webserver.hpp"
#include <fcntl.h>

Socket::Socket(unsigned short newport) : port(newport)
{

}

int Socket::getListenfd()
{
    return(this->listenfd);
}

bool	Socket::write_exit(std::string error)
{
	std::cout << "ERROR: " << error << std::endl;
	return (false);
}

bool	Socket::setUpConn(int kq, struct kevent evSet)
{
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //AF_INET = internet socket, SOCK_STREAM = tcp stream
		return (write_exit("socket error"));
	int reuse; //this and setsockopt avoids the bind error and allows to reuse the address
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
 		return(write_exit("reuse port error"));
	//setting up address you're listening on
	std::memset(&servAddr, '\0', sizeof(servAddr));
	servAddr.sin_family		= AF_INET;//AF_INET is an address family that is used to designate the type of addresses that your socket can communicate with (in this case, IPv4 addresses)
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // will respond to anything
	printf("port: %d\n", this->port);
	servAddr.sin_port		= htons(port); //port you're listening on
	if ((bind(listenfd, (SA *) &servAddr, sizeof(servAddr))) < 0)//bind listening socket to address
		return (write_exit("bind error"));
	if ((listen(listenfd, 10)) < 0)
		return (write_exit("listen error"));
    // if (fcntl(listenfd, F_SETFL, O_NONBLOCK) < 0)
   	// 	return (write_exit("fcntl error"));
	EV_SET(&evSet, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);//EV_SET is a macro that fills the kevent struct
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		return (write_exit("kqueue/kevent error"));
	// watchLoop();
	return (true);	

}