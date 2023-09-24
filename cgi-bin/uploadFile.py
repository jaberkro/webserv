#!/usr/bin/python3			# TO BE DELETED BEFORE SUBMISSION
#!/usr/local/bin/python3

import cgi, sys, os #, urllib.parse
import cgitb # for debugging messages
import time # for testing

cgitb.enable()
exitCode = 0
response = ""

print("PYTHON SCRIPT STARTED", file=sys.stderr) # DEBUG - TO BE DELETED

# time.sleep(10)

# if the upload directory does not exist, it will be created
uploadDir = os.getenv("UPLOAD_DIR")
if os.path.exists(uploadDir) == False or os.path.isdir(uploadDir) == False:
	os.mkdir(uploadDir, mode = 0o755)
	
form = cgi.FieldStorage()
if 'file' in form:
	fileToUpload = form['file']
	fileName = uploadDir + "/" + fileToUpload.filename
	open(fileName, 'wb').write(fileToUpload.file.read())
	with open("data/www/uploaded.html", 'r') as body:
		responseBody = body.read()
		response = os.environ["SERVER_PROTOCOL"] + " 201 Created\r\n"
		response += "Content-Length: {}\r\n".format(len(responseBody))		
		response += "Location: {}\r\n\r\n".format(fileName)
		response += responseBody
else:
	exitCode = 400

print("[script] about to send the following response:  ", response, file=sys.stderr) # DEBUG - TO BE DELETED
sys.stdout.buffer.write(response.encode())
sys.stdin.close()
sys.stdout.close()

print("PYTHON SCRIPT FINISHED, exit code is ", exitCode, file=sys.stderr) # DEBUG - TO BE DELETED
sys.exit(exitCode)
