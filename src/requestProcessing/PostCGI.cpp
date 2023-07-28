#include "PostCGI.hpp"
#include "Request.hpp"
#include <iostream>
#include <unistd.h>	// for pipe, fork, execve
#include <cstring>	// for strdup
#include <string>	// for to_string
#include <exception>

PostCGI::PostCGI(Request req) : _req(req)
{
	arg = new char*[NUM_OF_ARGS];
	sizeEnv = 0;
	size_t	i;
	extern char	**environ;
	
	while (environ[sizeEnv])
		sizeEnv++;
	env = new char*[sizeEnv + 18];
	arg[0] = strdup("cgi-bin/uploadFile.py"); //Need this as arg with constructor!
	std::cout << "script: [" << arg[0] << "]" << std::endl;
	arg[3] = strdup("TESTFILE2.txt"); //ˆˆ
	arg[4] = NULL;
	for (i = 0; i < sizeEnv; i++)
		env[i] = strdup(environ[i]);
	env[i++] = strdup("PATH_INFO=cgi-bin/uploadFile.py"); //ˆˆ
	std::map<std::string, std::string> _reqHeaders = _req.getHeaders();
	std::string contentLengthStr = "CONTENT_LENGTH=" + _reqHeaders["Content-Length"];
	std::cout << "The contentlengthstr is: [" << contentLengthStr.c_str() << "]" << std::endl;
	std::string contentTypeStr = "CONTENT_TYPE=" + _reqHeaders["Content-Type"];
	std::cout << "The contentTypestr is: [" << contentTypeStr.c_str() << "]" << std::endl;
	
	env[i++] = strdup(contentLengthStr.c_str());
	env[i++] = strdup(contentTypeStr.c_str());
	env[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[i++] = strdup(("REMOTE_HOST=" + _reqHeaders["Host"]).c_str());
	env[i++] = strdup("SCRIPT_FILENAME=cgi-bin/uploadFile.py");
	env[i++] = strdup("SCRIPT_NAME=uploadFile.py");
	env[i++] = strdup("REQUEST_METHOD=POST");
	env[i++] = strdup("UPLOAD_DIR=data/uploads/");
	//Should check and adjust the env following
	env[i++] = strdup("HTTP_COOKIE=");
	env[i++] = strdup("HTTP_USER_AGENT=");
	env[i++] = strdup("QUERY_STRING=");
	env[i++] = strdup("REMOTE_ADDR=");
	env[i++] = strdup("SERVER_NAME=webserv");
	env[i++] = strdup("SERVER_SOFTWARE=");
	env[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	env[i++] = strdup("PATH_TRANSLATED=cgi-bin/uploadFile.py");

	env[i] = NULL;
}

PostCGI::~PostCGI()
{

}

void	PostCGI::run(Request const & _req)//misschien vectorpair laten returnen met info voor response? Afh. van wat script returns
{
	// const char	*msg = "Hi from the parent process!";
	char	*buf = new char[LEN + 1];
	ssize_t	readBytes = 0;

	try 
	{

		if (pipe(webservToScript) < 0 || pipe(scriptToWebserv) < 0)
			throw std::runtime_error("Pipe failed");
		//std::cout << "script: [" << arg[0] << "]" << std::endl;
		arg[0] = strdup("cgi-bin/uploadFile.py"); //Need this as arg with constructor!
		arg[1] = strdup(std::to_string(webservToScript[R]).c_str());
		arg[2] = strdup(std::to_string(scriptToWebserv[W]).c_str());
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
			if (execve(arg[0], arg, env) < 0)
				std::cerr << strerror(errno) << std::endl;
			std::cout << "script: [" << arg[0] << "]" << std::endl;
		}
		else
		{
			close(scriptToWebserv[W]);
			close(webservToScript[R]);
			std::cout << "FULLBODY IN CGI.RUN FUNC: [" << _req.getBody() << "]" << std::endl;
			write(webservToScript[W], _req.getBody().c_str(), _req.getBody().size());// static_cast<const void *>(msg), strlen(msg));
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
	
	//jma: one of those deletes is too much
	// for (size_t i = 0; i < NUM_OF_ARGS; i++)
	// 	delete arg[i]; //causes error
	delete[] arg;
	while (env[sizeEnv])
		delete env[sizeEnv++];
	delete[] env;
	delete[] buf;
}

std::string	PostCGI::getResponse()
{
	return(this->response);
}