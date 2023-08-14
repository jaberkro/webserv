#!/usr/bin/python3

import cgi, sys, os, urllib.parse
import cgitb # for debugging messages


# the top line on Darina's laptop needs to be
#!/usr/bin/python3
# on Codam iMac:
#!/usr/local/bin/python3

cgitb.enable()



uploadDir = os.getenv("UPLOAD_DIR")
contentLen = int(os.getenv("CONTENT_LENGTH"))
contentType = os.getenv("CONTENT_TYPE")
print("PYTHON SCRIPT STARTED; content type is ", contentType, file=sys.stderr)

# chunk = 100
# while (True):
# 	read = sys.stdin.buffer.read(chunk)
# 	total += len(read)
# 	print("PYTHON just read {}, in total {}".format(len(read), total), file=sys.stderr)
# 	print("PYTHON: ", type(read), file=sys.stderr)
# 	print("PYTHON: ", read, file=sys.stderr)
# 	if len(read) == 0:
# 		break
print("PYTHON SCRIPT content length is ", contentLen, file=sys.stderr)

# IF UPLOADING THROUGH THE BROWSER: BODY IS MULTIPART/FORM-DATA 
if "multipart/form-data" in contentType:
	form = cgi.FieldStorage()
	# print("Form keys:", form.keys(), file=sys.stderr)  # Print the keys present in the form
	# print("Fileitem keys:", fileitem.keys(), file=sys.stderr)  # Print the keys present in the form
	if 'file' in form:
		fileToUpload = form['file']
		filename = "newUpload" + str(contentLen) if 'filename' not in fileToUpload else fileToUpload.filename
		print("File to upload:", filename, file=sys.stderr)
		fileName = os.path.basename(filename)			# THIS TO BE FURTHER UNDERSTOOD!!!!!
		open(uploadDir + fileName, 'wb').write(fileToUpload.file.read())
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

with open("data/www/uploaded.html", 'r') as uploaded:
	responseBody = uploaded.read()
	# response = "{} 201 Created\r\nContent-Type: text/html\r\n\r\nUpload successful.".format(os.environ["PROTOCOL_VERSION"])
	response = "201 Created\r\nContent-Type: text/html\r\nContent-Length: {}\r\nLocation: {}\r\n\r\n".format(len(responseBody), uploadDir + fileName) + responseBody # Location to be fixed! And content type
	sys.stdout.buffer.write(response.encode())

sys.stdin.close()
sys.stdout.close()

print("PYTHON SCRIPT FINISHED", file=sys.stderr)
