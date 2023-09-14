#!/usr/local/bin/python3
# !/usr/bin/python3

import os, sys

fileName = os.getenv("UPLOAD_DIR") + "/" + os.getenv("QUERY_STRING")[5:]

response = ""
responseBody = ""
exitCode = 0

print("Get script started", file=sys.stderr)

# checking whether the file exists
if not os.path.isfile(fileName):
	exitCode = 404

# checking whether the file has read permissions
elif not os.access(fileName, os.R_OK):
	exitCode = 403

# if all is ok, send the selected file:
else:
	with open(fileName, 'rb') as image:
		response = os.environ["SERVER_PROTOCOL"] + " 200 OK\r\n"
		response += "Content-Type: {}\r\n".format("image/" + fileName[fileName.find('.') + 1:])
		response += "Content-Length: {}\r\n\r\n".format(len(responseBody))
		print("[script] response: ", response, file=sys.stderr)
		sys.stdout.buffer.write(response.encode())
		while 1:
			responseBody = image.read(5000)
			if not responseBody:
				break
			sys.stdout.buffer.write(responseBody)

sys.stdin.close()
sys.stdout.close()
sys.exit(exitCode)