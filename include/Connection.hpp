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
	private:
		Request			*_newReq;
		Response		*_newResp;
		Server			*_handler;
		int				_listenfd;
		std::string		_address;
		unsigned short	_port;
		//std::string	_serverNames; BS: Deze nodig?
	public:
		void			handleRequest(int connfd, std::vector<Server> servers);//, Server *& handler, Request *& newReq);
		void			handleResponse();//Request *& newReq, Response *& newResp, Server *& handler);//int connfd, std::vector<Server> servers)
		void			setRequest(Request *request);
		Request *		getRequest(void);

		Connection();
		Connection(int listenfd, Socket sckt);
		Connection(const Connection & src);
		Connection& operator=(const Connection &src);
		~Connection();
};



#endif
