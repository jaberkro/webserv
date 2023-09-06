#!/usr/bin/python3

#!/usr/local/bin/python3

import cgi, sys, os #, urllib.parse
import cgitb # for debugging messages

cgitb.enable()

def sendResponseSuccess(fileName):
	with open("data/www/uploaded.html", 'r') as uploaded:
		responseBody = uploaded.read()
		response = "{} 201 Created\r\nContent-Type: text/html\r\nContent-Length: {}\r\nLocation: {}\r\n\r\n".format(os.environ["SERVER_PROTOCOL"], len(responseBody), uploadDir + fileName) + responseBody 
	sys.stdout.buffer.write(response.encode())

# print("PYTHON SCRIPT STARTED", file=sys.stderr)

# if the upload directory does not exist, it will be created
uploadDir = os.getenv("UPLOAD_DIR")
if os.path.exists(uploadDir) == False or os.path.isdir(uploadDir) == False:
	os.mkdir(uploadDir, mode = 0o755)

contentLen = os.getenv("CONTENT_LENGTH")
contentType = os.getenv("CONTENT_TYPE")
	
form = cgi.FieldStorage()
if 'file' in form:
	fileToUpload = form['file']
	fileName = fileToUpload.filename
	open(uploadDir + fileName, 'wb').write(fileToUpload.file.read())
	sendResponseSuccess(fileName)
else:
	redirect_url = "/postFailed.html"
	with open("data/www/postFailed.html", 'r') as uploaded:
		responseBody = uploaded.read()
		response = "{} 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: {}\r\n\r\n".format(os.environ["SERVER_PROTOCOL"], len(responseBody)) + responseBody
	sys.stdout.buffer.write(response.encode())

sys.stdin.close()
sys.stdout.close()

# print("PYTHON SCRIPT FINISHED", file=sys.stderr)
