/******************************
 Name: Joanna Lew
 Date: 8-13-17
 OSU 372 Networks
 Project 2: Create a simple file transfer program
    Server sends directory contents to client OR
    Server sends data of a file over to client
 ******************************/

#include <stdio.h>              // standard
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>          // sockets
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include <unistd.h>             // file directory
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>             // signals

int keepGoing = 0;              // bool for multiple connections until SIGINT

void catchSIGINT(){
    keepGoing = 1;
    return;
}

int main(int argc, const char * argv[]) {
    // number of arguments must be 3
    if (argc != 2){
        fprintf(stderr, "usage: ftserver port\n");
        exit(1);
    }
    
    // catch SIGINT
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = catchSIGINT;
    SIGINT_action.sa_flags = 0;
    sigemptyset(&SIGINT_action.sa_mask);
    if (sigaction(SIGINT, &SIGINT_action, NULL) == -1) {
        fprintf(stderr, "Sigaction Error");
        exit(1);
    }
    
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;
    int status;
    
    // load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // use IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    
    if ((status = getaddrinfo(NULL, argv[1], &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
        
    
    // make a socket, bind it, and listen on it:
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
        fprintf(stderr, "Error creating socket.\n");
        exit(1);
    }
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
        fprintf(stderr, "Error binding to port.\n");
        exit(1);
    }
    if (listen(sockfd, 1) == -1){
        fprintf(stderr, "Error listening on port.\n");
        exit(1);
    }
    else{
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
        printf("Server running on %s port %d\n", hostname, atoi(argv[1]));
    }
    
    // continue to get connections until SIGINT received
    while (keepGoing == 0){
        // now accept an incoming connection:
        addr_size = sizeof their_addr;
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) < 0){
            if (keepGoing == 1){
                break;
            }
            else{
                fprintf(stderr, "Error on accept\n");
                exit(1);
            }
        }
        
        // ready to communicate on socket descriptor new_fd
        // get command from client (either 'list' if '-l', or filename if '-g')
        char buffer[256];
        memset(buffer, '\0', 256);
        int charsRead = recv(new_fd, buffer, 255, 0);
        if (charsRead < 0){
            fprintf(stderr, "Error reading from socket\n");
            exit(1);
        }
        
        // if list command from client is received
        if (strncmp(buffer, "list", 4) == 0){
            // get current working directory
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            
            // send out file names in current directory
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir (cwd)) != NULL) {
                while ((ent = readdir (dir)) != NULL) {
                    if (send(new_fd, ent->d_name, strlen(ent->d_name), 0) < 0)
                        fprintf(stderr, "Failed to send filename\n");
                    if (send(new_fd, "\n", 1, 0) < 0)
                        fprintf(stderr, "Failed to send char to client\n");
                }
                closedir (dir);
            } else {
                fprintf(stderr, "Failed to open directory\n");
                exit(1);
            }
        }
        
        // if get command from client is received
        else{
            FILE *fp = fopen(buffer, "r");
            // if file not found, send "badfile" to client
            if (fp == NULL){
                if (send(new_fd, "badfile", 7, 0) < 0){
                    fprintf(stderr, "Failed to send error to client\n");
                }
            }
            // otherwise, if file found, send file data
            else{
                char file_buff[1024];
                memset(file_buff, '\0', 1024);
                
                while (fgets(file_buff, sizeof(file_buff), fp) != NULL){
                    if (send(new_fd, file_buff, strlen(file_buff), 0) < 0){
                        fprintf(stderr, "Failed to send file data\n");
                    }
                }
                close(fp);
            }
        }
        // close the current socket
        close(new_fd);
    }
    
    // close main socket
    printf("\nClosing socket and ending program\n");
    close(sockfd);
    
    // free address info linked list
    freeaddrinfo(res);
    
    return 0;
}

// References:
// Beej's guide: https://goo.gl/HTlom (setting up a socket on server's side)
// Brewster, B: OSU 344 Operating Systems (for signals & sockets)
// Stack Overflow: https://goo.gl/rypese (for getting directory contents)
