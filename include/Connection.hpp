#ifndef CONNECTION_HPP
# define CONNECTION_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Socket.hpp"

class Connection
{
	public:
	
		Connection();
		Connection(int listenfd, Socket sckt);
		Connection(const Connection & src);
		Connection& operator=(const Connection &src);
		~Connection();

    	void		handleRequest(int connfd, std::vector<Server> servers, int dataSize);
		void		handleResponse(int dataSize);
		void		setRequest(Request *request);
		Request *	getRequest(void);
		Response *	getResponse(void);
		int			getListenFd();
		bool		getTimer();
		void		setTimer(bool state);

	private:

		Request			*_newReq;
		Response		*_newResp;
		Server			*_handlingServer;
		int				_listenfd;
		std::string		_address;
		unsigned short	_port;
		bool			_timer;

		void	cleanUp(void);

};

#endif
