#!/usr/local/bin/python3

import os, sys

fileName = os.getenv("UPLOAD_DIR") + os.getenv("QUERY_STRING")[5:]

with open(fileName, 'rb') as image:
	responseBody = image.read()
	response = "{} 200 OK\r\nContent-Type: image/{}\r\nContent-Length: {}\r\n\r\n".format(os.environ["SERVER_PROTOCOL"], fileName[fileName.find('.') + 1:], len(responseBody)) 
	sys.stdout.buffer.write(response.encode())
	sys.stdout.buffer.write(responseBody)

# add code for when sth goes wrong

sys.stdin.close()
sys.stdout.close()