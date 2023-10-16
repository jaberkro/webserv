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
	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(address.c_str(), nullptr, &hints, &res) != 0)
		throw Socket::AddressConversionError();
	if (res != nullptr)
	{
		struct sockaddr_in *addr_in = (struct sockaddr_in*)res->ai_addr;
		_servAddr.sin_addr.s_addr = addr_in->sin_addr.s_addr;
		freeaddrinfo(res);
	}
	else
		throw Socket::AddressConversionError();
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
		if (error_code == EADDRINUSE)
		{
			if (close(_listenfd) < 0)
				std::cout << "Closing fd failed" << std::endl;
			throw SocketError();
		}
		else
			throw BindError();
	}
	else if ((listen(_listenfd, SOMAXCONN)) < 0)
		throw Socket::ListenError();
    if (fcntl(_listenfd, F_SETFL, O_NONBLOCK) < 0)
		throw SocketError();
	EV_SET(&evSet, _listenfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw Socket::KeventError();
}
