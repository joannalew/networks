/******************
 chatclient.c
 
 Name: Joanna Lew
 Date: 7-30-17
 Info: OSU CS 372 Project 1
 Descrip: Use TCP Protocol and Socket API to create
    a simple chat system between a client and host
********************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


/** Gets a string, checks length, and stores it in a variable
    Taken from https://stackoverflow.com/a/3723071 and edited
    @param prmpt is displayed on console
    @param buff is where string is stored
    @param sz is the max length of string
    @returns 0 if successful, 1 if too long, 2 if no input */
static int getLine (char *prmpt, char *buff, size_t sz) {
    int ch, extra;
    
    // Get line with buffer overrun protection.
    if (prmpt != NULL) {
        printf ("%s", prmpt);
        fflush (stdout);
    }
    
    if (fgets(buff, sz, stdin) == NULL)
        return 2;
    
    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? 1 : 0;
    }
    
    // Otherwise remove newline and give string back to caller.
    buff[strlen(buff)-1] = '\0';
    return 0;
}


/** Get a username that is 1-10 chars long, no spaces 
    @param buff is where the string is to be stored
    @param sz is the max size of the string */
void getUsername(char *buff, size_t sz){
    int rc = -1;
    
    // Continue until valid username entered
    while (rc != 0){
        rc = getLine ("Enter username: ", buff, sz);
        
        // No input
        if (rc == 2) {
            // Extra NL since system doesn't output that on EOF.
            printf("\nNo input\n");
        }
        
        // Too long
        if (rc == 1) {
            printf("Username too long. Maximum 10 characters.\n");
        }
        
        // Check if too short
        if (strcmp(buff, "") == 0){
            printf("Username too short. Minimum one character.\n");
            rc = -1;
        }
        
        // Check for multiple words
        char* pch = strchr(buff, ' ');
        if (pch != NULL){
            printf("Username must be one word.\n");
            rc = -1;
        }
    }
}

/** Prep socket address information
    Taken from https://goo.gl/M2mwDH (beej's guide) 
    @param hostname is the server's hostname or ip to connect to
    @param port is the server's port number to connect to
    @returns servinfo, a linked list of 1 or more struct addrinfos */
struct addrinfo *prepAddrInfo(char* hostname, char* port){
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    
    memset(&hints, 0, sizeof(hints));           // make sure struct is empty
    hints.ai_family = AF_INET;                  // IPv4
    hints.ai_socktype = SOCK_STREAM;            // TCP stream sockets
    
    // get the address information; store in servinfo
    if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    return servinfo;
}

/** Connect client and server with socket
    @param res is the addrinfo obtained from prepAddrInfo
    @returns sockFD, socket file descriptor */
int connectSocket(struct addrinfo *res){
    int sockFD;
    
    // create socket
    if ((sockFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
        fprintf(stderr, "Error creating socket.\n");
        exit(1);
    }
    
    // connect socket (connect client to server)
    if (connect(sockFD, res->ai_addr, res->ai_addrlen) == -1){
        fprintf(stderr, "Error connecting socket.\n");
        exit(1);
    }
    
    return sockFD;
}

/** Send client's username to server, obtain server's name
    @param sockFD is the socket file descriptor
    @param username is the client's username to send
    @param servname is the server's name received; changed from function */
void tradeNames(int sockFD, char *username, char *servname){
    int bytes_sent = send(sockFD, username, strlen(username), 0);
    if (bytes_sent == -1){
        fprintf(stderr, "Error sending username.\n");
        exit(1);
    }
    
    int bytes_recv = recv(sockFD, servname, 10, 0);
    if (bytes_recv == -1){
        fprintf(stderr, "Error getting server name");
        exit(1);
    }
}

/** Initiate simple chat
    Take turns sending messages (500 chars max), client first
    Stops when '\quit' is typed by either host or client
    @param sockFD is the socket file descriptor
    @param username is the client's username
    @param servname is the server's name */
void chat(int sockFD, char *username, char *servname){
    // create buffers for user and server messages; clear out mem
    char userMsg[501];
    char servMsg[501];
    memset(userMsg, 0, sizeof(userMsg));
    memset(servMsg, 0, sizeof(servMsg));
    
    int bytes_sent = 0;
    int bytes_recv = 0;
    
    while(1){
        // prompt client for message to send to server
        printf("%s> ", username);
        fgets(userMsg, 500, stdin);
        
        // if input is '\quit', break out of loop and close connection
        if (strncmp(userMsg, "\\quit", 5) == 0)
            break;
        
        // send message to server
        // don't send last character ('\n') for formatting
        bytes_sent = send(sockFD, userMsg, strlen(userMsg) - 1, 0);
        if (bytes_sent == -1){
            fprintf(stderr, "Error sending message to server.\n");
            exit(1);
        }
        
        // get message from server
        bytes_recv = recv(sockFD, servMsg, 500, 0);
        if (bytes_recv == -1){
            fprintf(stderr, "Error getting message from server.\n");
            exit(1);
        }
        else if (bytes_recv == 0){
            printf("Connection closed by server.\n");
            break;
        }
        else{
            printf("%s> %s\n", servname, servMsg);
        }
        
        // clear the memory and continue
        memset(userMsg, 0, sizeof(userMsg));
        memset(servMsg, 0, sizeof(servMsg));
    }
    
    // stop chat; close the connection
    close(sockFD);
    printf("Connection closed\n");
}


int main(int argc, const char *argv[]) {
    // number of arguments must be 3
    if (argc != 3){
        fprintf(stderr, "usage: chatclient hostname port\n");
        exit(1);
    }
    
    // get username
    char username[11];
    getUsername(username, sizeof(username));

    // setup to connect to server; get address info
    struct addrinfo *res = prepAddrInfo(argv[1], argv[2]);
    
    // connect to server
    int sockFD = connectSocket(res);
    
    // send username to server; get server name
    char servname[11];
    tradeNames(sockFD, username, servname);
    
    // do the chat w/ server
    chat(sockFD, username, servname);
    
    // free address info linked list
    freeaddrinfo(res);
    
    return 0;
}
