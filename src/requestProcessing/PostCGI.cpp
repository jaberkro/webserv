#include "PostCGI.hpp"
#include "Request.hpp"
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

void	PostCGI::prepareArg()
{
	this->_arg = new char*[2];
	this->_arg[0] = strdup("cgi-bin/uploadFile.py");
	this->_arg[1] = NULL;

	// std::cout << "* ARGUMENTS *" << std::endl;
	// size_t	i = 0;
	// while (this->_arg[i])
	// 	std::cout << this->_arg[i++] << std::endl;

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
	std::string contentTypeStr = "CONTENT_TYPE=" + reqHeaders["Content-Type"];
	std::string	queryString = this->_req.getQueryString();
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
	this->_env[i++] = strdup(("QUERY_STRING=" + queryString).c_str());
	this->_env[i++] = strdup("REMOTE_ADDR=");
	this->_env[i++] = strdup("SERVER_NAME=webserv");
	this->_env[i++] = strdup("SERVER_SOFTWARE=");
	this->_env[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	this->_env[i++] = strdup("PATH_TRANSLATED=cgi-bin/uploadFile.py");
	this->_env[i] = NULL;

	// std::cout << "* ENV *" << std::endl;
	// i = 0;
	// while (this->_env[i])
	// 	std::cout << this->_env[i++] << std::endl;
}


void	PostCGI::run()
{
	char	*buf = new char[LEN + 1];
	ssize_t	readBytes = 0;

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
			if (execve(this->_arg[0], this->_arg, this->_env) < 0)
				std::cerr << strerror(errno) << std::endl;
			std::cout << "FAIL: script: [" << this->_arg[0] << "]" << std::endl;
		}
		else
		{
			// sleep(10);
			// std::vector<std::pair<std::vector<uint8_t>, size_t> >	const & body = this->_req.getBody();
			std::string body = this->_req.getBody();
			// size_t		i = 1;

			// MAKE THEM NON-BLOCKING
			
			std::cout << "PARENT - BODY length of " << this->_req.getBodyLength() << " split into " << body.size() << " chunks" << std::endl;
			// std::cout << " ==============================BODY========================\n\n" << _req.getBody() << "\n\n==================================================================\n\n" <<std::endl;
			write(_webservToScript[W], _req.getBody().c_str(), _req.getBody().size());
			// for (auto it = body.begin(); it != body.end(); it++)
			// {
			// 	std::cout << "\tchunk no. " << i++ << " is " << it->second << " characters long and its size is " << it->first.size() << std::endl;
			// 	for (size_t i = 0; i < it->second; i++)
			// 	{
			// 		std::cout << it->first[i];
					// write(this->_webservToScript[W], &(it->first[0]), it->first.size());

			// 		// send(this->_webservToScript[W], &(it->first[i]), 1, 0);
			// 	}
			// 	std::cout << std::endl;
			// 	// write(this->_webservToScript[W], &(it->first[0]), it->first.size());
			// }
			close(this->_scriptToWebserv[W]);
			close(this->_webservToScript[R]);
			close(this->_webservToScript[W]);
			std::string	fullResponse;
			while ((readBytes = read(this->_scriptToWebserv[R], buf, LEN)) > 0)
			{
				buf[readBytes] = '\0';
				fullResponse.append(buf);
			}
			this->_response = fullResponse;
			send(this->_req.getConnFD(), this->_response.c_str(), this->_response.length(), 0);
			
			//buf[bytesRead] = '\0';
			// std::cout << "Parent received this message: " << buf << std::endl;
			std::cout << "Parent received this response: [" << this->_response << "]" << std::endl;
			close(this->_scriptToWebserv[R]);
			// send(this->_req.getConnFD(), this->_response.c_str(), this->_response.length(), 0);
			waitpid(id, &(this->_exitCode), 0);
			if (WIFEXITED(this->_exitCode))
				std::cout << "Script exited with exit code " << this->_exitCode << std::endl;
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
	return(this->_response);
}