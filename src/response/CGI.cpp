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

char	*CGI::protectedStrdup(std::string str, Response & response)
{
	char *newStr = strdup(str.c_str());
	if (newStr == NULL)
		response.setState(RES_ERROR);
	return (newStr);
}

void	CGI::prepareArg(std::string const & scriptName, Response & response)
{
	this->_arg = new char*[2];
	this->_arg[0] = protectedStrdup(scriptName.c_str(), response);
	this->_arg[1] = NULL;
}

void	CGI::addToEnv(size_t &i, std::string what, Response & response)
{
	this->_env[i++] = protectedStrdup(what.c_str(), response);
}

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
		this->_env[i] = protectedStrdup(environ[i], response);
	addToEnv(i, "PATH_INFO=" + response.getPathInfo(), response);
	addToEnv(i, "CONTENT_LENGTH=" + reqHeaders["Content-Length"], response);
	addToEnv(i, "CONTENT_TYPE=" + reqHeaders["Content-Type"], response);
	addToEnv(i, "GATEWAY_INTERFACE=CGI/1.1", response);
	addToEnv(i, "REMOTE_HOST=" + reqHeaders["Host"], response);
	addToEnv(i, "SCRIPT_FILENAME=" + scriptName, response);
	addToEnv(i, "SCRIPT_NAME=" + scriptName, response);
	addToEnv(i, "REQUEST_METHOD=" + this->_req.getMethod(), response);
	addToEnv(i, "UPLOAD_DIR=" + response.getLocation()->getUploadDir(), response);
	addToEnv(i, "HTTP_COOKIE=", response);
	addToEnv(i, "HTTP_USER_AGENT=", response);
	addToEnv(i, "QUERY_STRING=" + this->_req.getQueryString(), response);
	addToEnv(i, "REMOTE_ADDR=", response);
	addToEnv(i, "SERVER_NAME=webserv", response);
	addToEnv(i, "SERVER_SOFTWARE=", response);
	addToEnv(i, "SERVER_PROTOCOL=HTTP/1.1", response);
	addToEnv(i, "PATH_TRANSLATED=" + scriptName, response);
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
