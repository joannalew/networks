OSU CS372 Project 1
Name: Joanna Lew
Date: 7-30-17
Descrip: Create a simple chat system with a chat server (Python)
    and chat client (C) using TCP protocol and sockets API.

================================================
Instructions:
1. run ./chatserver.py <portNumber>
   change permissions if necessary with:
      chmod +x chatserver.py

2. compile chatclient.c with: 
      gcc -o client chatclient.c
   (ignore the wave of warnings)

3. execute client & connect to server with:
      ./client <serverName> <serverPort>

4. client chooses a username (1-10 chars) and 
   initiates the conversation; server and client 
   take turns sending messages

5. close connection by inputting '\quit'

6. quit program with SIGINT on server (Cltr+C)
   
================================================
References:

C Client Setup & Examples:
Beej's Guide
http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#getaddrinfo

C Socket Examples:
Brewster, OSU CS 344 (Operating Systems I) 

Python Server Setup:
Kurose & Ross, Computer Networking, Section 2.7

Python Server Example:
Python Standard Library Documentation
https://docs.python.org/2/library/socket.html#example

================================================
Testing was performed on flip3

TCP SERVER

flip3 ~ 171% ./chatserver.py 32120
Connected by ('127.0.0.1', 44598)
Tom> a bird is walking on the tree
Server> it hums and sings and flaps its wings
Tom> i thought i saw it turn its head
Server> before it flapped its wings and flew away
Connection closed
Waiting for connection
Connected by ('127.0.0.1', 44624)
Bob123> take2
Server> \quit
Connection closed
Waiting for connection
^CTraceback (most recent call last):
  File "./chatserver.py", line 72, in <module>
    sockConnect, clientAddr = servSock.accept()
  File "/usr/lib64/python2.7/socket.py", line 202, in accept
    sock, addr = self._sock.accept()
KeyboardInterrupt

--------------------------------------------------

TCP CLIENT

flip3 ~ 153% ./client localhost 32120
Enter username: Tom
Tom> a bird is walking on the tree
Server> it hums and sings and flaps its wings
Tom> i thought i saw it turn its head
Server> before it flapped its wings and flew away
Tom> \quit
Connection closed
flip3 ~ 154% ./client localhost 32120
Enter username: Bob123
Bob123> take2
Connection closed by server.
Connection closed
