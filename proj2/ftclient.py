#!/bin/python


# Name: Joanna Lew
# Date: 8-13-17
# OSU CS 372 Networks
# Project 2: Create a simple file transfer program
#	Client either gets server's directory listing OR
#	Client requests a file to be obtained from server

import sys
import socket
import argparse
import os.path

# set up getting arguments from user
# returns arguments 
# usage: ftclient.py server_host server_port command (filename) data_port
def setup_args():
	# description and usage
	description='FTP Client'
	usage = 'ftclient.py server_host server_port command (filename) data_port'
	parser = argparse.ArgumentParser(description=description, usage=usage,
                                     epilog='-l and -g are mutually exclusive')

	# server host & server port
	parser.add_argument('server_host', 
						help='Hostname of the server to connect to')
	parser.add_argument('server_port', 
						type=int, choices=range(1025, 65535), metavar="server_port[1025-65535]",
						help='Port number of the server to connect to')

	# either '-l' (list directory) OR '-g filename' (get file)
	group = parser.add_mutually_exclusive_group(required=True)
	group.add_argument("-l", action="store_true",
						help='Display files available from server')
	group.add_argument("-g", action="store", nargs=1,
						help='Get contents of file from server',
						metavar='filename')

	# client port
	parser.add_argument('client_port',
						type=int, choices=range(1025, 65535), metavar="client_port[1025-65535]",
						help='Port number for the client to connect to')

	return parser.parse_args()

# creates a connection to server
# returns socket
def create_socket(server_name, server_port, client_port):
	# create a tcp/ip socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	# connect socket to port where the server is listening
	server_address = (server_name, server_port)
	print 'Connecting to %s port %s' % server_address
	try:
		sock.connect(server_address)
	except:
		print 'Error: Failed to connect to %s on port %s' % server_address
		sys.exit(1)

	return sock

	
if __name__ == "__main__":
	# get arguments and create socket
	args = setup_args()
	sock = create_socket(args.server_host, args.server_port, args.client_port)

	# if '-l' was chosen, send "list" to server
	# if '-g' was chosen, send filename to be "downloaded"
	if (args.l):
		command = "list"
	else:
		command = args.g[0]

	sock.send(command)

	getFile = False			# bool for whether to download file or not

	# if '-l' was chosen, print out filenames in server's directory
	if args.l:
		print "Files in server's directory:"
		foo = sock.recv(1024)
		while (foo):
			print foo
			foo = sock.recv(1024)

	# if '-g' was chosen
	else:
		# check that file exists in server's directory
		foo = sock.recv(1024)
		if foo == 'badfile':
			print "File not in server's directory"

		# check if file exists in client's directory
		# if exists, ask whether or not to overwrite current file
		else:
			if os.path.exists(args.g[0]):
				print "File exists"
				userInput = raw_input("Do you want to overwrite current file (y/n)?  ")
				if userInput == 'y' or userInput == 'Y' or userInput == 'yes' or userInput == 'Yes':
					getFile = True
			else:
				getFile = True

			# get file from server if user says "yes" or file doesn't exist on client
			# otherwise, default behavior is to NOT get file
			if getFile:
				print "Getting file: %s" % args.g[0]
				file = open(args.g[0], 'wb')
				while (foo):
					file.write(foo)
					foo = sock.recv(1024)
				file.close()
				print "Finished getting file"

	# close socket
	print "Closing connection"
	sock.close()


# References:
# argparse example (stack overflow): https://goo.gl/Yr4qcJ
# python client example: https://pymotw.com/2/socket/tcp.html#echo-client
# file transfer example (stack overflow): https://goo.gl/G3fKmP
