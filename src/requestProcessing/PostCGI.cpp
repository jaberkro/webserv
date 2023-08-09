#include "PostCGI.hpp"
#include "Request.hpp"
#include <iostream>
#include <unistd.h>	// for pipe, fork, execve
#include <cstring>	// for strdup
#include <string>	// for to_string
#include <exception>

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

void	PostCGI::prepareArg()
{
	this->_arg = new char*[2];
	this->_arg[0] = strdup("cgi-bin/uploadFile.py");
	this->_arg[1] = NULL;

	std::cout << "* ARGUMENTS *" << std::endl;
	size_t	i = 0;
	while (this->_arg[i])
		std::cout << this->_arg[i++] << std::endl;

}

void	PostCGI::prepareEnv()
{
	size_t		sizeEnv = 0;
	size_t		i;
	extern char	**environ;
	
	while (environ[sizeEnv])
	sizeEnv++;

	this->_env = new char*[sizeEnv + 18];
	for (i = 0; i < sizeEnv; i++)
		this->_env[i] = strdup(environ[i]);
	this->_env[i++] = strdup("PATH_INFO=cgi-bin/uploadFile.py"); //ˆˆ
	std::map<std::string, std::string> & reqHeaders = this->_req.getHeaders();
	std::string contentLengthStr = "CONTENT_LENGTH=" + reqHeaders["Content-Length"];
	// std::cout << "The contentlengthstr is: [" << contentLengthStr.c_str() << "]" << std::endl;
	std::string contentTypeStr = "CONTENT_TYPE=" + reqHeaders["Content-Type"];
	// std::cout << "The contentTypestr is: [" << contentTypeStr.c_str() << "]" << std::endl;
	this->_env[i++] = strdup(contentLengthStr.c_str());
	this->_env[i++] = strdup(contentTypeStr.c_str());
	this->_env[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	this->_env[i++] = strdup(("REMOTE_HOST=" + reqHeaders["Host"]).c_str());
	this->_env[i++] = strdup("SCRIPT_FILENAME=cgi-bin/uploadFile.py");
	this->_env[i++] = strdup("SCRIPT_NAME=uploadFile.py");
	this->_env[i++] = strdup("REQUEST_METHOD=POST");
	this->_env[i++] = strdup("UPLOAD_DIR=data/uploads/");
	//Should check and adjust the env following
	this->_env[i++] = strdup("HTTP_COOKIE=");
	this->_env[i++] = strdup("HTTP_USER_AGENT=");
	this->_env[i++] = strdup("QUERY_STRING=");
	this->_env[i++] = strdup("REMOTE_ADDR=");
	this->_env[i++] = strdup("SERVER_NAME=webserv");
	this->_env[i++] = strdup("SERVER_SOFTWARE=");
	this->_env[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	this->_env[i++] = strdup("PATH_TRANSLATED=cgi-bin/uploadFile.py");
	this->_env[i] = NULL;

	std::cout << "* ENV *" << std::endl;
	i = 0;
	while (this->_env[i])
		std::cout << this->_env[i++] << std::endl;
}


void	PostCGI::run()//misschien vectorpair laten returnen met info voor response? Afh. van wat script returns
{
	// const char	*msg = "Hi from the parent process!";
	char	*buf = new char[LEN + 1];
	ssize_t	readBytes = 0;

	try 
	{

		if (pipe(webservToScript) < 0 || pipe(scriptToWebserv) < 0)
			throw std::runtime_error("Pipe failed");
		id = fork();
		if (id < 0)
			throw std::runtime_error("Fork failed");
		if (id == 0)
		{
			close(webservToScript[W]);
			close(scriptToWebserv[R]);
			dup2(webservToScript[R], STDIN_FILENO);
			close(webservToScript[R]);
			dup2(scriptToWebserv[W], STDOUT_FILENO);
			close(scriptToWebserv[W]);
			if (execve(this->_arg[0], this->_arg, this->_env) < 0)
				std::cerr << strerror(errno) << std::endl;
			std::cout << "FAIL: script: [" << this->_arg[0] << "]" << std::endl;
		}
		else
		{
			std::string	chunk;
			std::string	const & body = this->_req.getBody();
			size_t		chunkSize = 1000;
			size_t		i = 0;
			
			std::cout << "PARENT - BODY length of " << body.length() << ": [" << body << "]" << std::endl;
			
			while (i * chunkSize < body.length())
			{
				chunk = body.substr(i++ * chunkSize, chunkSize);
				std::cout << "chunk is >" << chunk << "<" << std::endl;
				write(webservToScript[W], chunk.c_str(), chunk.length());
				chunk.clear();
				std::cout << "end of loop: i is " << i << ", i * chunkSize = " << i * chunkSize << ", body length is "<< body.length() << std::endl;
			}
			// write(webservToScript[W], _req.getBody().c_str(), _req.getBody().length());// static_cast<const void *>(msg), strlen(msg));
			close(scriptToWebserv[W]);
			close(webservToScript[R]);
			// std::cout << "FULLBODY IN CGI.RUN FUNC: [" << _req.getBody() << "]" << std::endl;
			// write(webservToScript[W], req.getBody().c_str(), req.getBody().size());// static_cast<const void *>(msg), strlen(msg));
			close(webservToScript[W]);
			std::string fullResponse;
			while ((readBytes = read(scriptToWebserv[R], buf, LEN)) > 0)
			{
				buf[readBytes] = '\0';
				fullResponse.append(buf);
			}
			response = fullResponse;
			
			//buf[bytesRead] = '\0';
			// std::cout << "Parent received this message: " << buf << std::endl;
			std::cout << "Parent received this response: [" << response << "]" << std::endl;
			close(scriptToWebserv[R]);
			waitpid(id, &exitCode, 0);
			if (WIFEXITED(exitCode))
				std::cout << "Script exited with exit code " << exitCode << std::endl;
		}
	}
	catch (std::runtime_error &re)
	{
		std::cerr << re.what() << std::endl;
	}
	delete[] buf;
}

std::string	PostCGI::getResponse()
{
	return(this->response);
}