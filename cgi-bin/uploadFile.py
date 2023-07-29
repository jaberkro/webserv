#!/usr/local/bin/python3

import cgi, sys, os
import cgitb # for debugging messages


# # required environment variables: UPLOAD_DIR and PROTOCOL_VERSION


cgitb.enable()


print("PYTHON SCRIPT STARTED", file=sys.stderr)

# Read the entire request (headers + body) from stdin
# full_request = sys.stdin.buffer.read()

# Print the received request (for debugging purposes)
# print("Received request:")
# print(full_request)
# print("environment vars")
# print(os.environ)

uploadDir = os.getenv("UPLOAD_DIR")
# uploadDir = os.environ['UPLOAD_DIR']
# envLen = os.getenv("CONTENT_LENGTH")
# print("Content-Length is:")
# print(envLen)
form = cgi.FieldStorage()
# print("===================================================")

# print(form)
# for key in form.keys():
# 	value = form[key].value
# 	print(f"Key: {key}, Value: {value}")
# print("===================================================")


fileitem = form['file']
# print("File is ") #, form['file'], file=sys.stderr)
# print(fileitem.filename)




# print("Form keys:", form.keys())  # Print the keys present in the form
if 'file' in form:
	fileToUpload = form['file']
	# print("File to upload:", fileToUpload)
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

