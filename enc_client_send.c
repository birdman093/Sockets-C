#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFFSIZE 256
#define ERROR_KEY '!'
#define DELIM_KEY '@'

int enc_client_send(int fd, int socket) 
{
    //read text into buffer and send to server
    ssize_t nread;
    int charsWritten;
    char buffer[BUFFSIZE];
    memset(buffer, '\0', sizeof(buffer));
    while ((nread = read(fd, buffer, BUFFSIZE-1)) > 0) {
        charsWritten = send(socket, buffer, strlen(buffer), 0);
        if (charsWritten < 0) {
            perror("CLIENT: ERROR writing to socket");
            return -1;
        }
        if (charsWritten < strlen(buffer)){
            // add write to stdout
            printf("CLIENT: WARNING: Not all data written to socket!\n");
        }
        memset(buffer, '\0', sizeof(buffer));
    }
    close(fd);
    // send break now
    charsWritten = send(socket, ERROR_KEY, 1, 0);
    if (charsWritten < 0) {
        perror("CLIENT: ERROR writing to socket");
        return -1;
    }
    if (charsWritten < strlen(buffer)){
        // add write to stdout
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }
    return 0;
    
}

