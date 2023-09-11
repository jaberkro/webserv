#include "../include/Socket.hpp"
#include "../include/Webserver.hpp"
#include <fcntl.h>

int Socket::getListenfd() const
{
    return(this->_listenfd);
}

unsigned short Socket::getPort() const
{
	return (this->_port);
}

std::string Socket::getAddress() const
{
	return (this->_address);
}

/**
 * @brief Sets up address, port, and host the socket needs to listen to.
 * - AF_INET is an address family used to designate the type of address the socket
 *   will listen to (which is in our case, IPv4)
 * 
 * @param address the address to listen to
 */

void	Socket::setAddressHostPort(std::string address)
{
	std::memset(&_servAddr, '\0', sizeof(_servAddr)); // CHECK IF FAILS, INTERNAL_SERVER_ERROR
	_servAddr.sin_family		= AF_INET;
	_servAddr.sin_port		= htons(_port);
	printf("port: [%d] address: [%s]\n", this->_port, address.c_str());
	// servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // will respond to anything
	///Checken met curl --resolve of onderstaande code nodig is, of bovenstaande regel genoeg is!////
	// Als onderstaande niet nodg is, dan ook std::string address niet meer doorsturen naar deze func!
	/////begin/////
	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints)); // CHECK IF FAILS, INTERNAL_SERVER_ERROR
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(address.c_str(), nullptr, &hints, &res) != 0)
		throw Socket::AddressConversionError(); // INTERNAL_SERVER_ERROR
	if (res != nullptr)
	{
		//Copy resolved IP to servAddr
		struct sockaddr_in *addr_in = (struct sockaddr_in*)res->ai_addr;
		_servAddr.sin_addr.s_addr = addr_in->sin_addr.s_addr;
		freeaddrinfo(res);
	}
	else
		throw Socket::AddressConversionError();  // INTERNAL_SERVER_ERROR
	////////end////////

}

/**
 * @brief Construct a new Socket:: Socket object, and bind it, make it listen and add it to the kqueue
 * Small notes: - AF_INET = internet socket, SOCK_STREAM = tcp stream
 * 				- the setsockopt function avoids in this case a bind error and allows to reuse the address
 * 				- SOMAXCONN is the max. possible connections on your system (usually 128)
 * 
 * @param address the address to listen on
 * @param newport the port to listen on
 * @param kq the fd of the kqueue
 * @param evSet the kevent struct used to add information to the kqueue
 */

Socket::Socket(std::string address, unsigned short newport, int kq, struct kevent evSet) : _port(newport), _address(address)
{
	if ((_listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw Socket::SocketError();
	int reuse = 1;
	if (setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
		throw Socket::SetsockoptError();
	setAddressHostPort(address);
	if ((bind(_listenfd, (SA *) &_servAddr, sizeof(_servAddr))) < 0)
	{
		int error_code = errno;
		if (error_code == EADDRINUSE) //deze error niet tonen, iet catchen bij EADDRINUSE
			std::cout <<
			"(Above mentioned address is already in use, socket will not be initialized)"
			<< std::endl;
		else
			throw BindError();
	}
	if ((listen(_listenfd, SOMAXCONN)) < 0)
		throw Socket::ListenError();
    // if (fcntl(listenfd, F_SETFL, O_NONBLOCK) < 0)
   	// 	return (write_exit("fcntl error"));
	EV_SET(&evSet, _listenfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw Socket::KeventError();
	// struct timespec timeout;
	// timeout.tv_sec = 10; //Timeout after 10 sec
	// timeout.tv_nsec = 0;//this is nanosecs
	// EV_SET(&evSet, 0, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 0, NULL);
	// if (kevent(kq, &evSet, 1, NULL, 0, &timeout) == -1)
	// 	throw Socket::KeventError();
}
