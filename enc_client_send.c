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
#define ERROR_STR "!"
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
        if (buffer[strlen(buffer)-1] == '\n') {
            buffer[strlen(buffer)-1] = DELIM_KEY;
        }
        charsWritten = send(sd_client, buffer, strlen(buffer), 0);
        if (charsWritten < 0) {
            perror("CLIENT: ERROR writing file data to server");
            return -1;
        }
        // Handles insufficient character writing error to server 
        if (charsWritten != strlen(buffer)) {
            while (charsWritten != strlen(buffer)) {
                // send error message to notify server
                charsWritten = send(sd_client, ERROR_STR, 1, 0);
                if (charsWritten < 0) {
                    perror("CLIENT: ERROR sending data to socket after insufficient amount receieved by server");
                    return -1;
                }

                // resend the message
                charsWritten = send(sd_client, buffer, strlen(buffer), 0);
                if (charsWritten < 0) {
                    perror("CLIENT: ERROR sending data to socket after insufficient amount receieved by server");
                return -1;
                } 
            }
        }
            
        memset(buffer, '\0', sizeof(buffer));
    }
    close(fd_txt);
    return 0;
    
}

