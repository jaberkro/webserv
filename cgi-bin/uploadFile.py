#!/usr/bin/python3

#!/usr/local/bin/python3

import cgi, sys, os #, urllib.parse
import cgitb # for debugging messages

print("hoooooi", file=sys.stderr)

cgitb.enable()

def sendResponseSuccess(fileName):
	redirect_url = "/uploaded.html"
	with open("data/www/uploaded.html", 'r') as uploaded:
		responseBody = uploaded.read()
		response = "{} 302 Found\r\nContent-Type: text/html\r\nContent-Length: {}\r\nLocation: {}\r\n\r\n".format(os.environ["SERVER_PROTOCOL"], len(responseBody), redirect_url) + responseBody # Location to be fixed! And content type
	sys.stdout.buffer.write(response.encode())


uploadDir = os.getenv("UPLOAD_DIR")
if os.path.exists(uploadDir) == False or os.path.isdir(uploadDir) == False:
	os.mkdir(uploadDir, mode = 0o755)

contentLen = os.getenv("CONTENT_LENGTH")
contentType = os.getenv("CONTENT_TYPE")
print("PYTHON SCRIPT STARTED", file=sys.stderr) #; content type is ", contentType, ", content length is ", contentLen, file=sys.stderr)

# IF UPLOADING THROUGH THE BROWSER: BODY IS MULTIPART/FORM-DATA 
# if "multipart/form-data" in contentType:
	
print("Before loading FieldStorage", file=sys.stderr)
form = cgi.FieldStorage()
print("After loading FieldStorage", file=sys.stderr)
print("Form keys:", form.keys(), file=sys.stderr)  # Print the keys present in the form
if 'file' in form:
	fileToUpload = form['file']
	fileName = fileToUpload.filename
	print("File to upload:", fileName, file=sys.stderr)
	open(uploadDir + fileName, 'wb').write(fileToUpload.file.read())	#IF UPLOAD DIR DOES NOT EXIST, CREATE IT
		# with open(uploadDir + fileName, 'wb') as f:
	# 	f.write(fileToUpload.file.read())
	sendResponseSuccess(fileName)
else:
	print("File key not found in form!", file=sys.stderr)
	redirect_url = "/postFailed.html"
	with open("data/www/postFailed.html", 'r') as uploaded:
		responseBody = uploaded.read()
		response = "{} 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: {}\r\nLocation: {}\r\n\r\n".format(os.environ["SERVER_PROTOCOL"], len(responseBody), redirect_url) + responseBody # Location to be fixed! And content type
	sys.stdout.buffer.write(response.encode())

# IF UPLOADING THROUGH CURL
# else:
# 	fileName = "newUpload" + str(contentLen)
# 	totalRead = 0
# 	with open(uploadDir + fileName, 'wb') as f:
# 		while True:
# 			data = sys.stdin.buffer.read(1024)  # Read data in chunks of 1024 bytes
# 			print("[python] just read >", data, "<", file=sys.stderr)
# 			totalRead += len(data)
# 			if data:
# 				if "application/x-www-form-urlencoded" in contentType:
# 					urllib.parse.unquote_to_bytes(data)
# 				f.write(data)
# 			else:
# 				break
# 			if totalRead == contentLen:
# 				break
		# sendResponseSuccess(fileName)

sys.stdin.close()
sys.stdout.close()

print("PYTHON SCRIPT FINISHED", file=sys.stderr)
