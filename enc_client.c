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


#define MAXCONN 5
#define ENC_CLIENT "enc_client"
#define ENC_SERVER "enc_server"
#define BUFFSIZE 256
#define TEXT_INPUT 1
#define KEY_INPUT 2
#define PORT_INPUT 3

int main(int argc, char *argv[]) 
{
    // Handle invalid input: too many or too few inputs
    if (argc > 4) {
        printf("Too Many arguments\n");
        return EXIT_FAILURE;
    } else if (argc != 4) {
        printf("Not enough arguments!\n");
        return EXIT_FAILURE;
    }

    // client socket
    int sd_client = socket(AF_INET, SOCK_STREAM, 0);
    if (sd_client == -1) {
        printf("CLIENT: ERROR, creating Socket\n");
        return EXIT_FAILURE;
    }

    // client socket address struct
    struct sockaddr_in clientAddress;
    memset((char*) &clientAddress, '\0', sizeof(clientAddress));
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(atoi(argv[PORT_INPUT]));
    struct hostent *hostInfo = gethostbyname(ENC_SERVER);
    if (hostInfo == NULL) { 
        perror("CLIENT: ERROR, no such host\n"); 
        return EXIT_FAILURE; 
    }
    memcpy((char*) &clientAddress.sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);

    // connect to server
    int ret_Conn = connect(sd_client, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
    if (ret_Conn < 0) {
        perror("CLIENT: ERROR connecting");
    }

    // get info from plaintext, loop through until buffer is empty, load 255 characters
    char buffer[BUFFSIZE];
    int fd_plaintext = open(argv[TEXT_INPUT],O_RDONLY,0);
    if (fd_plaintext < 0) {
        perror("CLIENT: ERROR, reading plain-text");
    }

    ssize_t nread;
    int charsWritten;
    while ((nread = read(fd_plaintext, buffer, BUFFSIZE-1)) > -1) {
        // send info to server
        charsWritten = send(sd_client, buffer, strlen(buffer), 0);
        if (charsWritten < 0) {
            perror("CLIENT: ERROR writing to socket");
        }
        if (charsWritten < strlen(buffer)){
            printf("CLIENT: WARNING: Not all data written to socket!\n");
        }
        memset(buffer, '\0', sizeof(buffer));
    }
    close(fd_plaintext);

    // add termination message

    //receive info from server
    memset(buffer, '\0', sizeof(buffer));
    int charsRead;
    charsRead = recv(sd_client, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        perror("CLIENT: ERROR reading from socket");
    }
    printf("CLIENT: I received this from the server: \"%s\"\n", buffer);


    close(sd_client);    
    return EXIT_SUCCESS;
}

