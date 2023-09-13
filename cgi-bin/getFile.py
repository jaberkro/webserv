#!/usr/local/bin/python3
# !/usr/bin/python3

import os, sys

fileName = os.getenv("UPLOAD_DIR") + "/" + os.getenv("QUERY_STRING")[5:]

response = ""
responseBody = ""
exitCode = 0

print("Get script started", file=sys.stderr)

def createResponse(statusCode, message, contentType, contentLength):
	global response 
	response = "{} {} {}\r\nContent-Type: {}\r\nContent-Length: {}\r\n\r\n".format(os.environ["SERVER_PROTOCOL"], statusCode, message, contentType, contentLength)

def sendResponse(statusCode, message, contentType, contentLength):
	createResponse(statusCode, message, contentType, contentLength)
	print("Response to be sent: ", response, file=sys.stderr)
	sys.stdout.buffer.write(response.encode())


# checking whether the file exists
if not os.path.isfile(fileName):
	print("*File not found: ", fileName, file=sys.stderr)
	sendResponse(404, "Not Found", "html/text", 0)
	exitCode = 404

# checking whether the file has read permissions
elif not os.access(fileName, os.R_OK):
	print("*Forbidden: ", fileName, file=sys.stderr)
	sendResponse(403, "Forbidden", "html/text", 0)
	exitCode = 403

# if all is ok, send the selected file:
else:
	print("*File OK", file=sys.stderr)
	with open(fileName, 'rb') as image:
		responseBody = image.read()
		sendResponse(200, "OK", "image/" + fileName[fileName.find('.') + 1:], len(responseBody))
		sys.stdout.buffer.write(responseBody)

sys.stdin.close()
sys.stdout.close()
sys.exit(exitCode)