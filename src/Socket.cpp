#include "Socket.hpp"
#include "Webserver.hpp"
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

void	Socket::setAddressHostPort(std::string address)
{
	std::memset(&_servAddr, '\0', sizeof(_servAddr));
	_servAddr.sin_family		= AF_INET;
	_servAddr.sin_port		= htons(_port);
	printf("port: [%d] address: [%s]\n", this->_port, address.c_str());
	// servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // will respond to anything
	///Checken met curl --resolve of onderstaande code nodig is, of bovenstaande regel genoeg is!////
	// Als onderstaande niet nodg is, dan ook std::string address niet meer doorsturen naar deze func!
	/////begin/////
	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
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
}
