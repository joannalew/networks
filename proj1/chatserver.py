#!/bin/python

# Joanna Lew
# 7-30-17
# OSU CS372 Project 1
# Descrip: Using TCP Protocol and Socket API, create a 
#	simple chat system between a client and a server.

# Python Server Example: https://docs.python.org/2/library/socket.html#example


import sys
import socket

# Send server name to client; get client name
# sockConnect is the socket to send & receive data on the connection
# serverName is the server's name to send to client
# returns client's name as variable
def tradeNames(sockConnect, serverName):
	clientName = sockConnect.recv(1024)
	sockConnect.send(serverName)

	return clientName

# Initiate simple chat
# sockConnect is the socket to send & receive data on the connection
# clientName is the client's username
# serverName is the server's name
def chat(sockConnect, clientName, serverName):
	servData = ""

	while 1:
		# get data from client; print it out
		recvData = sockConnect.recv(512)
		if recvData == "":
			print "Connection closed\nWaiting for connection"
			break

		print("{}> {}".format(clientName, recvData))

		# prompt user for message to send to client
		servData = ""
		while len(servData) == 0 or len(servData) > 500:
			servData = raw_input("{}> ".format(servName))

		# stop loop (to close connection) if '\quit' received
		if servData == "\quit":
			print "Connection closed\nWaiting for connection"
			break

		sockConnect.send(servData)


if __name__ == "__main__":
	# number of arguments must be 2
	if len(sys.argv) != 2:
		print "usage: chatserver port"
		exit(1)

	# get port number; create IPv4 stream socket and bind to port
	servPort = int(sys.argv[1])
	servSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	servSock.bind(('', servPort))

	# listen for input (from one connection); set server name
	servSock.listen(1)
	servName = "Server"

	while 1:
		# accept a connection
		# store the socket as sockConnect; print out address of client
		sockConnect, clientAddr = servSock.accept()
		print "Connected by", clientAddr

		# initiate chat until '\quit' received; close connection
		# but continue waiting for new connection until SIGINT received
		# SIGINT not gracefully handled (error messages spewed)
		chat(sockConnect, tradeNames(sockConnect, servName), servName)
		sockConnect.close()



