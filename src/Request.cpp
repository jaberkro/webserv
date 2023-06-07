#include "Request.hpp"
#include "../include/server.hpp"
#include <sys/socket.h>


Request::Request(int connfd) : _connfd (connfd) {}

Request::~Request(void) {}

/* 
	This function parses each line of the request and saves the data in the 
	corresponding member variables
 */
void	Request::parse(std::string line) {

	std::cout << "Parsing line \"" << line << "\"" << std::endl;
}

/* 
PARSING
	- trim trailing spaces
	- replace multiple LWS (linear white space) by 1 SP
	- check chars (ASCII 0-31 are invalid and can be replaced by a SP, 
		unless in a quoted string)

 */

/* 
	This function reads a request from the socket, splits it into separate lines 
	and sends each line to the parsing function for further processing
 */
void	Request::processReq(void) {

	char		socketBuffer[MAXLINE];
	std::string	processingBuffer, line;
	int			n = 0;
	size_t		nl = 0;

	std::memset(socketBuffer, 0, MAXLINE);
	while ((n = recv(this->_connfd, &socketBuffer, MAXLINE - 1, 0))) {
		processingBuffer += socketBuffer;
		while ((nl = processingBuffer.find_first_of('\n')) < std::string::npos) {
			line = processingBuffer.substr(0, nl - 1);
			this->parse(line);
			processingBuffer.erase(0, nl + 1);
		}
		if (socketBuffer[n - 1] == '\n')
			break ;
	}
}
