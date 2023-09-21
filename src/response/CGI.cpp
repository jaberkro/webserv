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
#include <signal.h> //for killing child

CGI::CGI(Request & req) : _req(req), id(-1) {}

CGI::~CGI()
{

}

CGI &	CGI::operator=(CGI &r)
{
	this->_req = r._req;
	this->_arg = r._arg;
	this->_env = r._env;
	this->id = r.id;
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

/* DM: I am going to see if I can rewrite this to get rid of strdup() */
void	CGI::prepareArg(std::string const & scriptName)
{
	this->_arg = new char*[2];
	this->_arg[0] = strdup(scriptName.c_str());
	this->_arg[1] = NULL;
}

/* DM: I am going to see if I can rewrite this to get rid of strdup() */
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

int		CGI::checkTimeoutChild()
{
	auto	currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedSeconds = currentTime - _startTimeChild;
	
	if (elapsedSeconds.count() > 5.0)
		return (-1);
	return (0);
}

void	CGI::cgiWrite(Response & response, int dataSize)
{
	static bool	pipeFull = false;
	ssize_t 	bytesSent;
	ssize_t 	chunkSize = std::min(this->_req.getBody().length(), \
		static_cast<size_t>(MAXLINE));
	if (checkTimeoutChild() < 0)
	{
		// std::cerr << "TIMEOUT FOR CHILD PROCESS" << std::endl;
		kill(id, SIGKILL);
		response.setFilePath("");
		response.setError(REQUEST_TIMEOUT);
		return ;
	}
	if (bytesSent = write(_webservToScript[W], this->_req.getBody().c_str(), \
	chunkSize) == 0 || this->_req.getBody().size() == 0)
	{
		response.setState(READ_CGI);
		close(this->_scriptToWebserv[W]);
		close(this->_webservToScript[R]);
		close(this->_webservToScript[W]);
	}
	// bytesSent = write(_webservToScript[W], this->_req.getBody().c_str(), \
	// 	chunkSize);
	else if (bytesSent > 0)
	{
		this->_req.setBody(this->_req.getBody().erase(0, bytesSent));
		pipeFull = bytesSent == chunkSize ? false : true;
		std::cerr << "[writing to cgi] chunk size is " << chunkSize << ", BytesSent is " << bytesSent << " pipeFull is now " << pipeFull << std::endl;
	}
	// if (bytesSent == chunkSize)
	// {
	// 	// std::cerr << "Pipe is not full!" << std::endl;
	// 	pipeFull = false;
	// }
	// else if (bytesSent > 0 && bytesSent < chunkSize)
	// {
	// 	std::cerr << "Pipe is full now!" << std::endl;
	// 	pipeFull = true;
	// }
	// if (bytesSent > 0) //JMA: this if statement is important!
		// this->_req.setBody(this->_req.getBody().erase(0, bytesSent));
	// if (this->_req.getBody().size() == 0 || bytesSent == 0)
	// {
	// 	// std::cerr << "[writing to cgi] body size is " << this->_req.getBody().size() << ", BytesSent is " << bytesSent << std::endl;
	// 	// response.setState(READ_CGI);
	// 	// close(this->_scriptToWebserv[W]);
	// 	// close(this->_webservToScript[R]);
	// 	// close(this->_webservToScript[W]);
	// 	// std::cout << "Closing webservToScript[W]" << std::endl;
	// }

	if (bytesSent < 0 && pipeFull == false)
	{
		std::cerr << "oepsieWrite -- errno is " << errno << std::endl;

		close(this->_scriptToWebserv[R]);
		close(this->_scriptToWebserv[W]);
		close(this->_webservToScript[R]);
		close(this->_webservToScript[W]);
		kill(id, SIGKILL);
		response.setError(INTERNAL_SERVER_ERROR);
		response.setFilePath("");
	}
}

void	CGI::cgiRead(Response & response, std::string & fullResponse, int dataSize)
{
	static bool pipeEmpty = true;
	ssize_t 	bytesRead = 0;
	char		buf[RESPONSELINE];
	int			exitCode = 0;
	
	// close (this->_scriptToWebserv[R]); // this throws an error that is not catched anywhere

	if (checkTimeoutChild() < 0)
	{
		// std::cerr << "TIMEOUT FOR CHILD PROCESS" << std::endl;
		kill(id, SIGKILL);
		response.setError(REQUEST_TIMEOUT);
		response.setFilePath("");
		fullResponse.clear();
	}
	else if ((bytesRead = read(this->_scriptToWebserv[R], &buf, RESPONSELINE)) > 0) // why else if?
	{
		std::string	chunk(buf, bytesRead);
		response.addToFullResponse(chunk);
		std::cerr << "Read call for cgi, bytesRead = " << bytesRead << std::endl;
		pipeEmpty = bytesRead == RESPONSELINE ? false : true;
	}
	// if (bytesRead > 0 && bytesRead == RESPONSELINE)
	// {
	// 	// std::cerr << "read last bit of pipe for now" << std::endl;
	// 	pipeEmpty = false;
	// }
	// if (bytesRead >= 0 && bytesRead < RESPONSELINE)
	// {
	// 	std::cerr << "read last bit of pipe for now, bytesRead is " << bytesRead << std::endl;
	// 	pipeEmpty = true;
	// }
	if (bytesRead == 0 && (waitpid(id, &(this->_childProcessExitStatus), \
	WUNTRACED | WNOHANG) != 0))
	{
		close(this->_scriptToWebserv[R]);
		if (WIFEXITED(this->_childProcessExitStatus))
		{
			exitCode = WEXITSTATUS(this->_childProcessExitStatus);
			if (exitCode > 0)
			{
				response.setStatusCode(exitCode == 1 ? \
					INTERNAL_SERVER_ERROR : exitCode + 256);
				response.setFilePath("");
				fullResponse.clear();
			}
		}
		response.setState(PENDING);
		size_t	i = 0;
		while (this->_env[i])
			delete this->_env[i++];
		delete[] this->_env;
		delete this->_arg[0];
		delete[] this->_arg;
	}
	if (bytesRead < 0 && pipeEmpty == false)
	{
		std::cerr << "oepsieREad -- errno is " << errno << ", dataSize is " << dataSize << std::endl;
		close(this->_scriptToWebserv[R]);
		kill(id, SIGKILL);
		response.setStatusCode(INTERNAL_SERVER_ERROR);
		response.setState(RES_ERROR);
		response.setFilePath("");
		fullResponse.clear();
	}
}

void	CGI::run(Response & response)
{
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
			if (execve(this->_arg[0], this->_arg, this->_env) < 0)
			{
				std::cerr << strerror(errno) << std::endl;
				std::cerr << "FAIL: script: [" << this->_arg[0] << "]" << std::endl;
				// std::cerr << response.getRequest().getProtocolVersion() << " 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
				// std::cerr << response.getRequest().getProtocolVersion() << " 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
				// std::cerr << "just wrote in the pipe ? " << std::endl;
				exit(INTERNAL_SERVER_ERROR);
			}
		}
		else
		{
			_startTimeChild = std::chrono::system_clock::now();
			std::cerr << "In else statement of cgi call. State: " << response.getState() << /*", start time: " << _startTimeChild <<*/ std::endl;
			response.setState(WRITE_CGI);
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
