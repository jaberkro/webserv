#include "Response.hpp"
#include <unistd.h>
#include <fcntl.h>

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

void	CGI::closePipes(size_t whichPipes)
{
	close(this->_webservToScript[W]);
	close(this->_webservToScript[R]);
	close(this->_scriptToWebserv[W]);
	if (whichPipes == R)
		close(this->_scriptToWebserv[R]);
}

char	*CGI::protectedStrdup(std::string str)
{
	char *newStr = strdup(str.c_str());
	if (newStr == NULL)
		throw CGI::CgiError();
	return (newStr);
}

void	CGI::prepareArg(std::string const & scriptName)
{
	this->_arg = new char*[2];
	this->_arg[0] = protectedStrdup(scriptName.c_str());
	this->_arg[1] = NULL;
}

void	CGI::addToEnv(size_t &i, std::string what)
{
	this->_env[i++] = protectedStrdup(what.c_str());
}

char	**CGI::copyEnvironToEnv(char **environ, size_t &i)
{
	size_t	sizeEnv = 0;
	
	while (environ[sizeEnv])
		sizeEnv++;
	char	**newEnv = new char*[sizeEnv + 18];
	for (i = 0; i < sizeEnv; i++)
		newEnv[i] = this->protectedStrdup(environ[i]);
	return (newEnv);
}

void	CGI::prepareEnv(std::string const & scriptName, Response & response)
{
	size_t									i = 0;
	extern char								**environ;
	std::map<std::string, std::string> &	reqHeaders = this->_req.getHeaders();

	this->_env = this->copyEnvironToEnv(environ, i);
	addToEnv(i, "PATH_INFO=" + response.getPathInfo());
	addToEnv(i, "CONTENT_LENGTH=" + reqHeaders["Content-Length"]);
	addToEnv(i, "CONTENT_TYPE=" + reqHeaders["Content-Type"]);
	addToEnv(i, "GATEWAY_INTERFACE=CGI/1.1");
	addToEnv(i, "REMOTE_HOST=" + reqHeaders["Host"]);
	addToEnv(i, "SCRIPT_FILENAME=" + scriptName);
	addToEnv(i, "SCRIPT_NAME=" + scriptName);
	addToEnv(i, "REQUEST_METHOD=" + this->_req.getMethod());
	addToEnv(i, "UPLOAD_DIR=" + response.getLocation()->getUploadDir());
	addToEnv(i, "HTTP_COOKIE=");
	addToEnv(i, "HTTP_USER_AGENT=");
	addToEnv(i, "QUERY_STRING=" + this->_req.getQueryString());
	addToEnv(i, "REMOTE_ADDR=");
	addToEnv(i, "SERVER_NAME=webserv");
	addToEnv(i, "SERVER_SOFTWARE=");
	addToEnv(i, "SERVER_PROTOCOL=HTTP/1.1");
	addToEnv(i, "PATH_TRANSLATED=" + scriptName);
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
		closePipes(W);
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
	this->closePipes(R);
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
