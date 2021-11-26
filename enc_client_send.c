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
#define DELIM_STR "@"

int enc_client_send(int fd_txt, int sd_client) 
{
    //read text into buffer and send to server
    ssize_t nread;
    int charsWritten;
    char buffer[BUFFSIZE];
    memset(buffer, '\0', sizeof(buffer));
    while ((nread = read(fd_txt, buffer, BUFFSIZE-1)) > 0) {
        charsWritten = send(sd_client, buffer, strlen(buffer), 0);
        if (charsWritten < 0) {
            perror("CLIENT: ERROR writing to socket");
            return -1;
        }
        memset(buffer, '\0', sizeof(buffer));
    }
    close(fd_txt);
    // Send Delimiter to signify break
    charsWritten = send(sd_client, DELIM_STR, 1, 0);
    if (charsWritten < 0) {
        perror("CLIENT: ERROR sending stop message to socket");
        return -1;
    }
    return 0;
    
}

