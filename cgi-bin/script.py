#!/usr/local/bin/python3

# POST method
# 	The script MUST check the value of the CONTENT_LENGTH variable before reading the attached message-body, 
# 	and SHOULD check the CONTENT_TYPE value before processing it.

import os
import sys

# arguments structure: <script name> <FD read> <FD write> <new file name>
readFD = int(sys.argv[-3])
writeFD = int(sys.argv[-2])
uploadDir = os.environ["UPLOAD_DIR"]
newFileName = sys.argv[-1]

fromPipe = os.fdopen(readFD)
fileContent = fromPipe.read()
print("Script received the following message: ", fileContent)
fromPipe.close()

f = open(uploadDir + newFileName, 'w')	#later also use 'b' for binary
f.write(fileContent)
f.close()

toPipe = os.fdopen(writeFD, 'w')
toPipe.write("Hi from Python! Ssssss! I've created the file " + uploadDir + newFileName + " and put your message in it.")
toPipe.close()
