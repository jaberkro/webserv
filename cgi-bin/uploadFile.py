#!/usr/local/bin/python3

import cgi, sys, os #, urllib.parse
import cgitb # for debugging messages
import time # for testing

cgitb.enable()
exitCode = 0
response = ""

def responseSuccess(fileName):
	with open("data/www/uploaded.html", 'r') as body:
		responseBody = body.read()
		response = os.environ["SERVER_PROTOCOL"] + " 201 Created\r\n"
		response += "Content-Length: {}\r\n".format(len(responseBody))		
		response += "Location: {}\r\n\r\n".format(fileName)
		response += responseBody
	sys.stdout.buffer.write(response.encode())

# if the upload directory does not exist, it will be created
uploadDir = os.getenv("UPLOAD_DIR")
if os.path.exists(uploadDir) == False or os.path.isdir(uploadDir) == False:
	os.mkdir(uploadDir, mode = 0o755)

contentType = os.getenv("CONTENT_TYPE")	
if "multipart/form-data" in contentType:
	form = cgi.FieldStorage()
	if 'file' in form:
		fileToUpload = form['file']
		fileName = uploadDir + "/" + fileToUpload.filename
		open(fileName, 'wb').write(fileToUpload.file.read())
		responseSuccess(fileName)
	else:
		exitCode = 400
else:
	# print("[script] < WRITE THIS PART!!!!! here comes code that uploads a file> ", file=sys.stderr) # DEBUG - TO BE DELETED
	contentLen = os.getenv("CONTENT_LENGTH")
	fileName = "newUpload" + str(contentLen)
	totalRead = 0
	with open(uploadDir + "/" + fileName, 'wb') as f:
		while True:
			data = sys.stdin.buffer.read(1024)  # Read data in chunks of 1024 bytes
	# 			print("[python] just read >", data, "<", file=sys.stderr)
			totalRead += len(data)
			if data:
				if "application/x-www-form-urlencoded" in contentType:
					urllib.parse.unquote_to_bytes(data)
				f.write(data)
			else:
				break
			if totalRead == contentLen:
				break
		responseSuccess(fileName)

sys.stdin.close()
sys.stdout.close()

sys.exit(exitCode)
