#include "PostCGI.hpp"
#include <iostream>
#include <unistd.h>	// for pipe, fork, execve
#include <cstring>	// for strdup
#include <string>	// for to_string
#include <exception>
#include <cstdio>


PostCGI::PostCGI(Request & req) : _req(req) {}

PostCGI::~PostCGI()
{
	size_t	i = 0;
	while (this->_env[i])
		delete this->_env[i++];
	delete[] this->_env;
	delete this->_arg[0];
	delete[] this->_arg;
}

void	PostCGI::prepareArg(std::string const & scriptName)
{
	this->_arg = new char*[2];
	this->_arg[0] = strdup(scriptName.c_str()); // DM: this was hardcoded "cgi-bin/uploadFile.py"
	this->_arg[1] = NULL;

	std::cout << "* ARGUMENTS *" << std::endl;
	size_t	i = 0;
	while (this->_arg[i])
		std::cout << this->_arg[i++] << std::endl;
}

void	PostCGI::prepareEnv(std::string const & scriptName)
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
	this->_env[i++] = strdup("PATH_INFO=cgi-bin/uploadFile.py"); // TBD - what should this be?
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


void	PostCGI::run(Response & response)
{
	char	buf[RESPONSELINE];
	ssize_t	bytesRead = 0;

	try 
	{

		if (pipe(this->_webservToScript) < 0 || pipe(this->_scriptToWebserv) < 0)
			throw std::runtime_error("Pipe failed");
		id = fork();
		if (id < 0)
			throw std::runtime_error("Fork failed");
		if (id == 0)
		{
			std::cout << "Starting child process" << std::endl;
			close(this->_webservToScript[W]);
			close(this->_scriptToWebserv[R]);
			dup2(this->_webservToScript[R], STDIN_FILENO);
			close(this->_webservToScript[R]);
			dup2(this->_scriptToWebserv[W], STDOUT_FILENO);
			close(this->_scriptToWebserv[W]);
			std::cerr << "trying to run script: [" << this->_arg[0] << "]" << std::endl;
			response.setState(WRITE_CGI);
			if (execve(this->_arg[0], this->_arg, this->_env) < 0)
				std::cerr << strerror(errno) << std::endl;
			std::cout << "FAIL: script: [" << this->_arg[0] << "]" << std::endl;
		}
		else
		{
			std::string const & body = this->_req.getBody();
			// MAKE THEM NON-BLOCKING
			if (response.getState() == WRITE_CGI)
			{
				ssize_t bytesSent;
				ssize_t chunkSize = std::min(this->_req.getBody().length(), static_cast<size_t>(MAXLINE));
				bytesSent = write(_webservToScript[W], this->_req.getBody().c_str(), chunkSize);
				if (bytesSent < 0)
					std::cout << "BytesSent error, send 500 internal error" << std::endl;
				this->_req.getBody().erase(0, bytesSent);
				if (this->_req.getBody().size() == 0 || bytesSent == 0)
				{
					response.setState(READ_CGI);
					close(this->_scriptToWebserv[W]);
					close(this->_webservToScript[R]);
					close(this->_webservToScript[W]);
				}
			}
			if (response.getState() == WRITE_CGI)
			{
				if ((bytesRead = read(this->_scriptToWebserv[R], &buf, RESPONSELINE)) > 0)
				{
					std::string	chunk(buf, bytesRead);
					response.addToFullResponse(chunk);
				}
				// std::cout << "Parent received this response: [" << response.getFullResponse() << "]" << std::endl;
				if (bytesRead == 0)
				{
					close(this->_scriptToWebserv[R]);
					waitpid(id, &(this->_exitCode), 0); //BS: what if we don't wait for the script to finish, will the kq timer ring in time?
					if (WIFEXITED(this->_exitCode))
						std::cout << "Script exited with exit code " << this->_exitCode << std::endl;
					response.setState(PENDING);
				}
			}
		}
	}
	catch (std::runtime_error &re)
	{
		std::cerr << re.what() << std::endl;
		response.setStatusCode(INTERNAL_SERVER_ERROR);
	}
}

std::string	PostCGI::getResponse()
{
	return(this->_response);
}
