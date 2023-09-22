#include "CGI.hpp"
#include "Response.hpp"
#include <iostream>
#include <unistd.h>	// for pipe, fork, execve
#include <cstring>	// for strdup
#include <string>	// for to_string
#include <exception>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <ctime> 	//for std::time

CGI::CGI(Request & req) : _req(req), _id(-1) {}

CGI::~CGI() {}

CGI &	CGI::operator=(CGI &r)
{
	this->_req = r._req;
	this->_arg = r._arg;
	this->_env = r._env;
	this->_id = r._id;
	this->_webservToScript[R] = r._webservToScript[R];
	this->_webservToScript[W] = r._webservToScript[W];
	this->_scriptToWebserv[R] = r._scriptToWebserv[R];
	this->_scriptToWebserv[W] = r._scriptToWebserv[W];
	this->_childProcessExitStatus = r._childProcessExitStatus;
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

pid_t	CGI::getId()
{
	return (this->_id);
}


void	CGI::closePipes()
{
	close(this->_webservToScript[W]);
	close(this->_webservToScript[R]);
	close(this->_scriptToWebserv[W]);
	close(this->_scriptToWebserv[R]);
}

/* DM: I tried to see if I can rewrite this to get rid of strdup() 
but so far no luck (only segfaults and garbage value) */
void	CGI::prepareArg(std::string const & scriptName)
{
	this->_arg = new char*[2];
	this->_arg[0] = strdup(scriptName.c_str());
	this->_arg[1] = NULL;
}

/* DM: I tried to see if I can rewrite this to get rid of strdup() 
but so far no luck (only segfaults and garbage value) */
void	CGI::prepareEnv(std::string const & scriptName, Response & response)
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
	this->_env[i++] = strdup(("PATH_INFO=" + response.getPathInfo()).c_str());
	this->_env[i++] = strdup(("CONTENT_LENGTH=" + reqHeaders["Content-Length"]).c_str());
	this->_env[i++] = strdup(("CONTENT_TYPE=" + reqHeaders["Content-Type"]).c_str());
	this->_env[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	this->_env[i++] = strdup(("REMOTE_HOST=" + reqHeaders["Host"]).c_str());
	this->_env[i++] = strdup(("SCRIPT_FILENAME=" + scriptName).c_str());
	this->_env[i++] = strdup(("SCRIPT_NAME=" + scriptName).c_str());
	this->_env[i++] = strdup(("REQUEST_METHOD=" + this->_req.getMethod()).c_str());
	this->_env[i++] = strdup(("UPLOAD_DIR=" + response.getLocation()->getUploadDir()).c_str());
	this->_env[i++] = strdup("HTTP_COOKIE=");
	this->_env[i++] = strdup("HTTP_USER_AGENT=");
	this->_env[i++] = strdup(("QUERY_STRING=" + this->_req.getQueryString()).c_str());
	this->_env[i++] = strdup("REMOTE_ADDR=");
	this->_env[i++] = strdup("SERVER_NAME=webserv");
	this->_env[i++] = strdup("SERVER_SOFTWARE=");
	this->_env[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	this->_env[i++] = strdup(("PATH_TRANSLATED=" + scriptName).c_str());
	this->_env[i] = NULL;
}

void	CGI::checkTimeoutChild()
{
	auto	currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedSeconds = currentTime - _startTimeChild;
	
	if (elapsedSeconds.count() > 5.0)
		throw CgiScriptTimeout();
}

void	CGI::cgiWrite(Response & response)
{
	static bool	pipeFull = false;
	ssize_t 	bytesSent;
	ssize_t 	chunkSize = std::min(this->_req.getBody().length(), \
		static_cast<size_t>(MAXLINE));

	checkTimeoutChild();
	if ((bytesSent = write(_webservToScript[W], this->_req.getBody().c_str(), \
	chunkSize)) == 0 || this->_req.getBody().size() == 0)
	{
		response.setState(READ_CGI);
		close(this->_scriptToWebserv[W]);
		close(this->_webservToScript[R]);
		close(this->_webservToScript[W]);
	}
	else if (bytesSent > 0)
	{
		this->_req.setBody(this->_req.getBody().erase(0, bytesSent));
		pipeFull = bytesSent == chunkSize ? false : true;
	}
	else if (bytesSent < 0 && pipeFull == false)
		throw CgiError();
}

void	CGI::checkChildProcessExitCode(Response & response, \
	std::string & fullResponse)
{
	int	exitCode = 0;
	
	exitCode = WEXITSTATUS(this->_childProcessExitStatus);
	if (exitCode > 0)
	{
		response.setStatusCode(exitCode == 1 ? \
			INTERNAL_SERVER_ERROR : exitCode + 256);
		response.setFilePath("");
		fullResponse.clear();
	}
}

void	CGI::cleanUp(void)
{
	size_t	i = 0;

	while (this->_env[i])
		delete this->_env[i++];
	delete[] this->_env;
	delete this->_arg[0];
	delete[] this->_arg;
}

void	CGI::cgiRead(Response & response, std::string & fullResponse)
{
	static bool pipeEmpty = true;
	ssize_t 	bytesRead = 0;
	char		buf[RESPONSELINE];
	
	checkTimeoutChild();
	if ((bytesRead = read(this->_scriptToWebserv[R], &buf, RESPONSELINE)) > 0)
	{
		std::string	chunk(buf, bytesRead);
		response.addToFullResponse(chunk);
		pipeEmpty = bytesRead == RESPONSELINE ? false : true;
	}
	else if (bytesRead == 0 && \
	(waitpid(this->_id, &(this->_childProcessExitStatus), \
	WUNTRACED | WNOHANG) != 0))
	{
		close(this->_scriptToWebserv[R]);
		if (WIFEXITED(this->_childProcessExitStatus))
			this->checkChildProcessExitCode(response, fullResponse);
		response.setState(PENDING);
		this->cleanUp();
	}
	else if (bytesRead < 0 && pipeEmpty == false)
		throw CgiError();
}

void	CGI::executeScript()
{
	dup2(this->_webservToScript[R], STDIN_FILENO);
	dup2(this->_scriptToWebserv[W], STDOUT_FILENO);
	this->closePipes();
	if (execve(this->_arg[0], this->_arg, this->_env) < 0)
		exit(INTERNAL_SERVER_ERROR);
}

void	CGI::run(Response & response)
{
	try
	{
		if (pipe(this->_webservToScript) < 0 || pipe(this->_scriptToWebserv) < 0)
			throw std::runtime_error("Pipe failed");
		fcntl(this->_webservToScript[W], F_SETFL, O_NONBLOCK);
		fcntl(this->_scriptToWebserv[R], F_SETFL, O_NONBLOCK);
		response.setState(INIT_CGI);
		if ((this->_id = fork()) < 0)
			throw std::runtime_error("Fork failed");
		if (this->_id == 0)
			this->executeScript();
		else
		{
			_startTimeChild = std::chrono::system_clock::now();
			response.setState(WRITE_CGI);
		}
	}
	catch (std::runtime_error &re)
	{
		response.setStatusCode(INTERNAL_SERVER_ERROR);
	}
}

std::string	CGI::getResponse()
{
	return(this->_response);
}
