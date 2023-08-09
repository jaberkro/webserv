#!/usr/local/bin/python3

import cgi, sys, os
import cgitb # for debugging messages


# # required environment variables: UPLOAD_DIR and PROTOCOL_VERSION

# the top line on Darina's laptop needs to be
#!/usr/bin/python3

cgitb.enable()


print("PYTHON SCRIPT STARTED", file=sys.stderr)

uploadDir = os.getenv("UPLOAD_DIR")
contentLen = int(os.getenv("CONTENT_LENGTH"))

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

if "multipart/form-data" in os.getenv("CONTENT_TYPE"):
	form = cgi.FieldStorage()
	fileitem = form['file']
	if 'file' in form:
		fileToUpload = form['file']
		# print("File to upload:", fileToUpload)
		fileName = os.path.basename(fileToUpload.filename)
	else:
		print("File key not found in form!")
		# response = "{} 400 Bad Request\r\nContent-Type: text/html\r\n\r\nBad request.".format(os.environ["PROTOCOL_VERSION"])
		response = "400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: {}\r\n\r\nBad request.".format(12)
		sys.stdout.buffer.write(response.encode())
else:
	fileName = "hardcoded"

totalRead = 0
with open(uploadDir + fileName, 'wb') as f:
	while True:
		data = sys.stdin.buffer.read(1024)  # Read data in chunks of 1024 bytes
		totalRead += len(data)
		if data:
			f.write(data)
		if totalRead == contentLen:
			break
	# open(uploadDir + fileName, 'wb').write(fileToUpload.file.read())
	# response = "{} 201 Created\r\nContent-Type: text/html\r\n\r\nUpload successful.".format(os.environ["PROTOCOL_VERSION"])
	response = "201 Created\r\nContent-Type: text/html\r\nContent-Length: {}\r\n\r\nUpload successful.".format(18)
	sys.stdout.buffer.write(response.encode())



# print("Form keys:", form.keys())  # Print the keys present in the form

sys.stdin.close()
sys.stdout.close()

