OSU CS372 Project 2
Name: Joanna Lew
Date: 8-13-17
Descrip: Create a simple file transfer program with a server (C)
    and client (Python) using sockets API.

================================================
Instructions:

1. Compile ftserver.c with the following command:
	gcc -o ftserver ftserver.c
   (Ignore the warning)

2. Execute ftserver with the command:
	./ftserver portNumber

3. Run ftclient.py with the command:
	./ftclient.py serverHost serverPort [-l | -g filename] clientPort
   Choose ‘-l’ for a listing of files in server’s directory
   Choose ‘-g’ to download a file from server
   If necessary, change permissions with command:
	chmod +x ftclient.py

4. Repeat Step #3 as desired

5. To end program, send SIGINT to server (Cltr+C)

Note: ftclient.py and ftserver.c should be in separate directories
Note2: Files to be transferred should be given ‘read’ and ‘write’ permissions
	chmod 660 filename

================================================
References:

C Server Socket Setup & Examples:
Beej's Guide
http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#getaddrinfo

C Socket & Signal Examples:
Brewster, OSU CS 344 (Operating Systems I)

C Getting File Names in Directory:
Stack Overflow
https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c

Python Client Setup:
Python Module of the Week
https://pymotw.com/2/socket/tcp.html#echo-client

Python File Transfer Example:
Stack Overflow
https://stackoverflow.com/questions/27241804/sending-a-file-over-tcp-sockets-in-python

Python Argparse Example:
Stack Overflow
https://stackoverflow.com/questions/5603364/how-to-code-argparse-combinational-options-in-python

================================================
Testing was performed on flip2 (server) and flip3 (client)

CLIENT

flip3 ~/372-ftp-c 343% ./ftclient.py flip2.engr.oregonstate.edu 32200 -g some.txt 32201
Connecting to flip2.engr.oregonstate.edu port 32200
File exists
Do you want to overwrite current file (y/n)?  y
Getting file: some.txt
Finished getting file
Closing connection

——————————————————————————————————

SERVER

flip2 ~/372-ftp-s 243% ./ftserver 32200
Server running on flip2.engr.oregonstate.edu port 32200
^C
Closing socket and ending program

