#include "CGI.hpp"
#include "Response.hpp"
#include <iostream>
#include <unistd.h>	// for pipe, fork, execve
#include <cstring>	// for strdup
#include <string>	// for to_string
#include <exception>
#include <cstdio>
#include <fcntl.h>


CGI::CGI(Request & req) : _req(req), id(-1) {}

CGI::~CGI()
{

		// size_t	i = 0;
		// while (this->_env[i])
		// 	delete this->_env[i++];
		// delete[] this->_env;
		// delete this->_arg[0];
		// delete[] this->_arg;
}

CGI &	CGI::operator=(CGI &r)
{
	this->_req = r._req;
	this->_arg = r._arg;
	this->_env = r._env;
	this->id = r.id;
	this->_webservToScript[0] = r._webservToScript[0];
	this->_webservToScript[1] = r._webservToScript[1];
	this->_scriptToWebserv[0] = r._scriptToWebserv[0];
	this->_scriptToWebserv[1] = r._scriptToWebserv[1];
	this->_exitCode = r._exitCode;
	this->_response = r._response;
	return (*this);
}

int*	CGI::getWebservToScript()
{
	return(this->_webservToScript);
}

int*	CGI::getScriptToWebserv()
{
	return(this->_scriptToWebserv);
}

bool	CGI::checkIfCgiPipe()
{
	if (this->_req.getConnFD() != this->_scriptToWebserv[0] || this->_req.getConnFD() != this->_webservToScript[1])
		return (false);
	else
		return (true);
}

void	CGI::prepareArg(std::string const & scriptName)
{
	this->_arg = new char*[2];
	this->_arg[0] = strdup(scriptName.c_str()); // DM: this was hardcoded "cgi-bin/uploadFile.py"
	this->_arg[1] = NULL;

	std::cout << "* ARGUMENTS *" << std::endl;
	size_t	i = 0;
	while (this->_arg[i])
		std::cout << this->_arg[i++] << std::endl;
}

void	CGI::prepareEnv(std::string const & scriptName, std::string const & pathInfo)
{
	size_t									sizeEnv = 0;
	size_t									i;
	extern char								**environ;
	std::map<std::string, std::string> &	reqHeaders = this->_req.getHeaders();
	
	while (environ[sizeEnv])
	sizeEnv++;
	this->_env = new char*[sizeEnv + 18];
	for (i = 0; i < sizeEnv; i++)
		this->_env[i] = strdup(environ[i]);
	this->_env[i++] = strdup(("PATH_INFO=" + pathInfo).c_str());
	this->_env[i++] = strdup(("CONTENT_LENGTH=" + reqHeaders["Content-Length"]).c_str());
	this->_env[i++] = strdup(("CONTENT_TYPE=" + reqHeaders["Content-Type"]).c_str());
	this->_env[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	this->_env[i++] = strdup(("REMOTE_HOST=" + reqHeaders["Host"]).c_str());
	this->_env[i++] = strdup(("SCRIPT_FILENAME=" + scriptName).c_str());	// DM: this was "SCRIPT_FILENAME=cgi-bin/uploadFile.py"
	this->_env[i++] = strdup(("SCRIPT_NAME=" + scriptName).c_str());	// DM: this was "SCRIPT_NAME=uploadFile.py"
	this->_env[i++] = strdup(("REQUEST_METHOD=" + this->_req.getMethod()).c_str());	// DM: this was "REQUEST_METHOD=POST"
	this->_env[i++] = strdup("UPLOAD_DIR=data/uploads/");
	//Should check and adjust the env following
	this->_env[i++] = strdup("HTTP_COOKIE=");
	this->_env[i++] = strdup("HTTP_USER_AGENT=");
	this->_env[i++] = strdup(("QUERY_STRING=" + this->_req.getQueryString()).c_str());
	this->_env[i++] = strdup("REMOTE_ADDR=");
	this->_env[i++] = strdup("SERVER_NAME=webserv");
	this->_env[i++] = strdup("SERVER_SOFTWARE=");
	this->_env[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	this->_env[i++] = strdup("PATH_TRANSLATED=cgi-bin/uploadFile.py"); // TBD - what is this?
	this->_env[i] = NULL;

	std::cout << "* ENV *" << std::endl;
	i = 0;
	while (this->_env[i])
		std::cout << this->_env[i++] << std::endl;
}

void	CGI::cgiWrite(Response & response)
{
	// if (response.getState() == WRITE_CGI && checkIfCgiPipe())
	// {
		ssize_t bytesSent;
		ssize_t chunkSize = std::min(this->_req.getBody().length(), static_cast<size_t>(MAXLINE));
		bytesSent = write(_webservToScript[W], this->_req.getBody().c_str(), chunkSize);
		// std::cout << "BytesSent is " << bytesSent << std::endl;
		if (bytesSent < 0)
			std::cout << "BytesSent error, send 500 internal error" << std::endl;
		else
			this->_req.setBody(this->_req.getBody().erase(0, bytesSent)); //JMA: was outside of else statement before
		if (this->_req.getBody().size() == 0 || bytesSent == 0)// || bytesSent == -1) // JMA: partly outcommented to prevent early quitting
		{
			response.setState(READ_CGI);
			close(this->_scriptToWebserv[W]);
			close(this->_webservToScript[R]);
			close(this->_webservToScript[W]);
			std::cout << "Closing webservToScript[W]" << std::endl;
			waitpid(id, &(this->_exitCode), 0);
		}
	// }
}

void	CGI::cgiRead(Response & response, std::string & fullResponse)
{
	ssize_t bytesRead = 0;
	char	buf[RESPONSELINE];
	// std::cout << "Starting Cgi read" << std::endl;
	// if (response.getState() == READ_CGI && checkIfCgiPipe())
	// {
		if ((bytesRead = read(this->_scriptToWebserv[R], &buf, RESPONSELINE)) > 0)
		{
			// std::cout << "Read call for cgi, bytesRead = " << bytesRead << std::endl;
			std::string	chunk(buf, bytesRead);
			response.addToFullResponse(chunk);
		}
		if (bytesRead == 0 && (waitpid(id, &(this->_exitCode), WUNTRACED | WNOHANG) != 0))
		{
			close(this->_scriptToWebserv[R]);
			if (WIFEXITED(this->_exitCode))
				std::cout << "Script exited with exit code " << this->_exitCode << std::endl;
			if (this->_exitCode > 0)
			{
				response.setStatusCode(INTERNAL_SERVER_ERROR);
				response.setFilePath("");
				fullResponse.clear();
			}
			response.setState(PENDING);
			size_t	i = 0;
			std::cout << "Cleaning up cgi vars" << std::endl;
			while (this->_env[i])
				delete this->_env[i++];
			delete[] this->_env;
			delete this->_arg[0];
			delete[] this->_arg;
		// std::cout << "End of cgiRead func, bytesRead = " << bytesRead << " , state is " << response.getState() << std::endl;
		}
}

void	CGI::run(Response & response)
{
	// char	buf[RESPONSELINE];
	// ssize_t	bytesRead = 0;

	try //when anything fails in this try block, remember to reset the state and handle correctly!
	{
		// if (id != 0)
		// {
		// if (response.getState() == PENDING)
		// {
			std::cout << "Piping and forking now" << std::endl;
			if (pipe(this->_webservToScript) < 0 || pipe(this->_scriptToWebserv) < 0)
				throw std::runtime_error("Pipe failed");
			fcntl(this->_webservToScript[W], F_SETFL, O_NONBLOCK);
			fcntl(this->_scriptToWebserv[R], F_SETFL, O_NONBLOCK);
			std::cout << "FD VALUES OF PIPES:"
			<< " WebToScript[0] = [" << this->_webservToScript[0] << "] "
			<< " WebToScript[1] = [" << this->_webservToScript[1] << "] "
			<< " ScriptToWebs[0] = [" << this->_scriptToWebserv[0] << "] "
			<< " ScriptToWebs[1] = [" << this->_scriptToWebserv[1] << "] " << std::endl;

			response.setState(INIT_CGI);
			std::cout << "id = " << id << std::endl;
			id = fork();
			std::cerr << "id after fork = " << id << std::endl;

			if (id < 0)
				throw std::runtime_error("Fork failed");
		// }
		// }
		// else 
		// {
		if (id == 0)
		{
			// sleep(10);
			std::cout << "Starting child process, id = " << id << std::endl;
			close(this->_webservToScript[W]);
			close(this->_scriptToWebserv[R]);
			dup2(this->_webservToScript[R], STDIN_FILENO);
			close(this->_webservToScript[R]);
			dup2(this->_scriptToWebserv[W], STDOUT_FILENO);
			close(this->_scriptToWebserv[W]);
			std::cerr << "trying to run script: [" << this->_arg[0] << "]" << std::endl;
			// std::cerr << "Arg[0] = " << this->_arg[0] << " , * ALLE ARG VOOR EXECVE *" << std::endl;
			// int i = 0;
			// while (this->_arg[i])
			// 	std::cerr << this->_arg[i++] << std::endl;
			// std::cerr << "ENV:" << std::endl;
			// i = 0;
			// while (this->_env[i])
			// 	std::cerr << this->_env[i++] << std::endl << std::endl;

			if (execve(this->_arg[0], this->_arg, this->_env) < 0)
			{
				std::cerr << strerror(errno) << std::endl;
				std::cerr << "FAIL: script: [" << this->_arg[0] << "]" << std::endl;
				std::cerr << response.getRequest().getProtocolVersion() << " 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
				std::cout << response.getRequest().getProtocolVersion() << " 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
				std::cerr << "just wrote in the pipe ? " << std::endl;
			}
		}
		else
		{
			std::cerr << "In else statement of cgi call. State: " << response.getState() << std::endl;
			response.setState(WRITE_CGI);
			// std::string const & body = this->_req.getBody();
			// MAKE THEM NON-BLOCKING
			// if (response.getState() == WRITE_CGI && checkIfCgiPipe())
			// {
			// 	ssize_t bytesSent;
			// 	ssize_t chunkSize = std::min(this->_req.getBody().length(), static_cast<size_t>(MAXLINE));
			// 	bytesSent = write(_webservToScript[W], this->_req.getBody().c_str(), chunkSize);
			// 	std::cout << "BytesSent is " << bytesSent << std::endl;
			// 	if (bytesSent < 0)
			// 		std::cout << "BytesSent error, send 500 internal error" << std::endl;
			// 	this->_req.setBody(this->_req.getBody().erase(0, bytesSent)); //
			// 	if (this->_req.getBody().size() == 0 || bytesSent == 0)
			// 	{
			// 		response.setState(READ_CGI);
			// 		close(this->_scriptToWebserv[W]);
			// 		close(this->_webservToScript[R]);
			// 		close(this->_webservToScript[W]);
			// 		std::cout << "Closing webservToScript[W]" << std::endl;
			// 	}
			// }
			// if (response.getState() == READ_CGI && checkIfCgiPipe())
			// {
			// 	if ((bytesRead = read(this->_scriptToWebserv[R], &buf, RESPONSELINE)) > 0)
			// 	{
			// 		std::string	chunk(buf, bytesRead);
			// 		response.addToFullResponse(chunk);
			// 	}
				// std::cout << "Parent received this response: [" << response.getFullResponse() << "]" << std::endl;
				// if (bytesRead == 0)
				// {
			// if ((waitpid(id, &(this->_exitCode), WUNTRACED | WNOHANG)) != 0) //BS: what if we don't wait for the script to finish, will the kq timer ring in time?
			// {
			// 		close(this->_scriptToWebserv[R]);
			// 		if (WIFEXITED(this->_exitCode))
			// 			std::cout << "Script exited with exit code " << this->_exitCode << std::endl;
			// 		response.setState(PENDING);
					// size_t	i = 0;
					// std::cout << "Cleaning up cgi vars" << std::endl;
					// while (this->_env[i])
					// 	delete this->_env[i++];
					// delete[] this->_env;
					// delete this->_arg[0];
					// delete[] this->_arg;
			// }
				// }
			}
		}
	catch (std::runtime_error &re)
	{
		std::cerr << re.what() << std::endl;
		response.setStatusCode(INTERNAL_SERVER_ERROR);
	}
}

std::string	CGI::getResponse()
{
	return(this->_response);
}
