#!/usr/local/bin/python3

import cgi, os
import cgitb
import sys

# required environment variables: UPLOAD_DIR and PROTOCOL_VERSION

cgitb.enable()


print("PYTHON SCRIPT STARTED", file=sys.stderr)
uploadDir = os.environ["UPLOAD_DIR"]
form = cgi.FieldStorage()
# print("Filename is ", form['filename'], file=sys.stderr)

print("Form keys:", form.keys())  # Print the keys present in the form
if 'file' in form:
	fileToUpload = form['file']
	print("File to upload:", fileToUpload)
	fileName = os.path.basename(fileToUpload.filename)
	with open(uploadDir + fileName, 'wb') as f:
		while True:
			data = sys.stdin.buffer.read(1024)  # Read data in chunks of 1024 bytes
			if not data:
				break
			f.write(data)
	# open(uploadDir + fileName, 'wb').write(fileToUpload.file.read())
	# response = "{} 201 Created\r\nContent-Type: text/html\r\n\r\nUpload successful.".format(os.environ["PROTOCOL_VERSION"])
	response = "201 Created\r\nContent-Type: text/html\r\n\r\nUpload successful."
	sys.stdout.buffer.write(response.encode())
else:
	print("File key not found in form!")
	# response = "{} 400 Bad Request\r\nContent-Type: text/html\r\n\r\nBad request.".format(os.environ["PROTOCOL_VERSION"])
	response = "400 Bad Request\r\nContent-Type: text/html\r\n\r\nBad request."
	sys.stdout.buffer.write(response.encode())

sys.stdin.close()
sys.stdout.close()

# if fileToUpload.filename:
# 	fileName = os.path.basename(fileToUpload.filename)
# 	with open(uploadDir + fileName, 'wb') as f:
# 		while True:
# 			data = sys.stdin.buffer.read(1024)  # Read data in chunks of 1024 bytes
# 			if not data:
# 				break
# 			f.write(data)
# 	# open(uploadDir + fileName, 'wb').write(fileToUpload.file.read())
# 	response = "{} 201 Created\r\nContent-Type: text/html\r\n\r\nUpload successful.".format(
# 		os.environ["PROTOCOL_VERSION"]
# 	)
# 	sys.stdout.buffer.write(response.encode())
# else:
# 	response = "{} 400 Bad Request\r\nContent-Type: text/html\r\n\r\nBad request.".format(
# 		os.environ["PROTOCOL_VERSION"]
# 	)
# 	sys.stdout.buffer.write(response.encode())

# 	print("{} 201 Created".format(os.environ["PROTOCOL_VERSION"]))
# 	# perhaps more headers are required
# else:
# 	print("{} 400 Bad Request".format(os.environ["PROTOCOL_VERSION"]))