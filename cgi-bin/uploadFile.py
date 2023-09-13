#!/usr/local/bin/python3
# !/usr/bin/python3

import cgi, sys, os #, urllib.parse
import cgitb # for debugging messages

cgitb.enable()
exitCode = 0
response = ""

def createResponse(statusCode, message, contentFile, location):
	global response
	print("[script] about to open the content file ", contentFile, file=sys.stderr)
	with open(contentFile, 'r') as body:
		print("[script] file is open", file=sys.stderr)
		responseBody = body.read()
		print("[script] responseBody is ", responseBody, file=sys.stderr)
		response = "{} {} {}\r\nContent-Type: text/html\r\nContent-Length: {}\r\n".format(os.environ["SERVER_PROTOCOL"], statusCode, message, len(responseBody))
		print("[script] response is now ", response, "\nabout to add location ", location, file=sys.stderr)
		
		if len(location) > 0:
			response += "Location: {}\r\n".format(location)
		response += "\r\n" + responseBody
	print("[script] end of createRespone; response is now ", response, file=sys.stderr)
	


print("PYTHON SCRIPT STARTED", file=sys.stderr)

# if the upload directory does not exist, it will be created
uploadDir = os.getenv("UPLOAD_DIR")
if os.path.exists(uploadDir) == False or os.path.isdir(uploadDir) == False:
	os.mkdir(uploadDir, mode = 0o755)

contentLen = os.getenv("CONTENT_LENGTH")
contentType = os.getenv("CONTENT_TYPE")
	
form = cgi.FieldStorage()
if 'file' in form:
	fileToUpload = form['file']
	fileName = uploadDir + "/" + fileToUpload.filename
	print("[script] fileName is ", fileName, file=sys.stderr)
	open(fileName, 'wb').write(fileToUpload.file.read())
	print("[script] file size is ", os.path.getsize(fileName), file=sys.stderr)
	# close(fileName)
	print("[script] about to create a success message ", file=sys.stderr)
	createResponse(201, "Created", "data/www/uploaded.html", fileName)
else:
	exitCode = 400
	print("[script] about to create a failure message ", file=sys.stderr)
	createResponse(400, "Bad Request", "data/www/postFailed.html", "")


print("[script] about to send the following message:  ", response, file=sys.stderr)
sys.stdout.buffer.write(response.encode())
sys.stdin.close()
sys.stdout.close()

print("PYTHON SCRIPT FINISHED, exit code is ", exitCode, file=sys.stderr)
sys.exit(exitCode)
