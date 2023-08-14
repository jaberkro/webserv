#!/usr/local/bin/python3

# the top line on Darina's laptop needs to be
#!/usr/bin/python3
# on Codam iMac:
#!/usr/local/bin/python3

import cgi, sys, os, urllib.parse
import cgitb # for debugging messages

cgitb.enable()

def sendResponseSuccess(fileName):
	with open("data/www/uploaded.html", 'r') as uploaded:
		responseBody = uploaded.read()
		# response = "{} 201 Created\r\nContent-Type: text/html\r\n\r\nUpload successful.".format(os.environ["PROTOCOL_VERSION"])
		response = "201 Created\r\nContent-Type: text/html\r\nContent-Length: {}\r\nLocation: {}\r\n\r\n".format(len(responseBody), uploadDir + fileName) + responseBody # Location to be fixed! And content type
	sys.stdout.buffer.write(response.encode())


uploadDir = os.getenv("UPLOAD_DIR")
contentLen = int(os.getenv("CONTENT_LENGTH"))
contentType = os.getenv("CONTENT_TYPE")
print("PYTHON SCRIPT STARTED; content type is ", contentType, ", content length is ", contentLen, file=sys.stderr)

# IF UPLOADING THROUGH THE BROWSER: BODY IS MULTIPART/FORM-DATA 
if "multipart/form-data" in contentType:
	
	print("Before loading FieldStorage", file=sys.stderr)
	form = cgi.FieldStorage()
	print("After loading FieldStorage", file=sys.stderr)
	print("Form keys:", form.keys(), file=sys.stderr)  # Print the keys present in the form
	if 'file' in form:
		fileToUpload = form['file']
		print("FileToUpload:", fileToUpload, file=sys.stderr)
		fileName = "newUpload" + str(contentLen) if 'filename' not in fileToUpload else fileToUpload.filename
		print("File to upload:", fileName, file=sys.stderr)
		with open(uploadDir + fileName, 'wb') as f:
			f.write(fileToUpload.file.read())
		sendResponseSuccess(fileName)
	else:
		print("File key not found in form!", file=sys.stderr)
		# response = "{} 400 Bad Request\r\nContent-Type: text/html\r\n\r\nBad request.".format(os.environ["PROTOCOL_VERSION"])
		response = "400 Bad Request\r\nContent-Length: {}\r\n\r\nBad request.".format(12)
		sys.stdout.buffer.write(response.encode())

# IF UPLOADING THROUGH CURL
else:
	fileName = "newUpload" + str(contentLen)
	totalRead = 0
	with open(uploadDir + fileName, 'wb') as f:
		while True:
			data = sys.stdin.buffer.read(1024)  # Read data in chunks of 1024 bytes
			print("[python] just read >", data, "<", file=sys.stderr)
			totalRead += len(data)
			if data:
				if "application/x-www-form-urlencoded" in contentType:
					urllib.parse.unquote_to_bytes(data)
				f.write(data)
			else:
				break
			if totalRead == contentLen:
				break
		sendResponseSuccess(fileName)


sys.stdin.close()
sys.stdout.close()

print("PYTHON SCRIPT FINISHED", file=sys.stderr)
