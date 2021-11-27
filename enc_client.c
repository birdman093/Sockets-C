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
#define ENC_SERVER "localhost"
#define ENC_SERVER_STRING "enc_server"
#define BUFFSIZE 256
#define TEXT_INPUT 1
#define KEY_INPUT 2
#define PORT_INPUT 3
#define ERROR_KEY '!'
#define DELIM_KEY '@'
#define DELIM_STR "@"
#define ABORT_STR "$"

int enc_client_process(int, int, int); 
int enc_client_send(int, int);

int main(int argc, char *argv[]) 
{
    // Handle invalid input: too many or too few inputs
    if (argc > 4) {
        perror("CLIENT: Too Many arguments\n");
        return EXIT_FAILURE;
    } else if (argc != 4) {
        perror("CLIENT: Not enough arguments!\n");
        return EXIT_FAILURE;
    }

    // Client Socket and Addressing
    int sd_client = socket(AF_INET, SOCK_STREAM, 0);
    if (sd_client == -1) {
        perror("CLIENT: ERROR, creating Socket\n");
        return EXIT_FAILURE;
    }
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

    // Create connection to ENC_Server socket
    int ret_Conn = connect(sd_client, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
    if (ret_Conn < 0) {
        perror("CLIENT: ERROR connecting to server");
        return EXIT_FAILURE;
    }

    // Server Connection Validation Receive:  check server name
    char buffer[BUFFSIZE];
    memset(buffer, '\0', sizeof(buffer));
    int charsRead; int charsWritten;
    charsRead = recv(sd_client, buffer, sizeof(buffer) - 1, 0);
    if (charsRead < 0 ){
        perror("CLIENT: ERROR reading server validation from server socket");
        memset(buffer, '\0', sizeof(buffer));
        buffer[0] = ERROR_KEY;
        charsWritten = send(sd_client, buffer, strlen(buffer), 0);  
        exit(2);
    }
    else if (strcmp(buffer, ENC_SERVER_STRING) != 0) {
        perror("CLIENT: INVALID SERVER");
        memset(buffer, '\0', sizeof(buffer));
        buffer[0] = ERROR_KEY;
        charsWritten = send(sd_client, buffer, strlen(buffer), 0);  
        exit(2);
    }

    //Server Connection Validation Send: Confirm Acceptable Servercheck server name
    memset(buffer, '\0', sizeof(buffer));
    buffer[0] = DELIM_KEY;
    charsWritten = send(sd_client, buffer, strlen(buffer), 0);
    if (charsWritten < 0) {
            perror("CLIENT: ERROR writing server validation from server socket");
            exit(2);
    }

    // Plain Text and Key File Validation: Open files
    int fd_plaintext = open(argv[TEXT_INPUT],O_RDONLY,0);
    if (fd_plaintext < 0) {
        perror("CLIENT: ERROR, reading plain-text");
        exit(1);
    }
    int fd_keyfile = open(argv[KEY_INPUT],O_RDONLY,0);
    if (fd_keyfile < 0) {
        perror("CLIENT: ERROR, reading keyfile");
        close(fd_plaintext);
        exit(1);
    }

    // Plain Text and Key File Validation: Check for invalid characters, key file length greater than plaintext
    int ret_process = enc_client_process(fd_plaintext, fd_keyfile, sd_client);
    if (ret_process < 0) {
        charsWritten = send(sd_client, ABORT_STR, 1, 0);
        if (charsWritten < 0) {
            perror("CLIENT: ERROR notifying server of inadequate keyfile or plaintext file");
            exit(2);
        }
        close(fd_plaintext);
        close(fd_keyfile);
        close(sd_client); 
        exit(1);
    }

    // Data Transfer: Send key and plaintext to server, send DELIM_KEY to start, in between,  
    // at end to signify change of input
    charsWritten = send(sd_client, DELIM_STR, 1, 0);
    if (charsWritten < 0) {
        perror("CLIENT: ERROR sending start message to socket");
        return -1;
    }
    int ret_send_pt = enc_client_send(fd_plaintext, sd_client); 
    if (ret_send_pt < 0){
        perror("CLIENT: ERROR sending keyfile to socket");
        exit(1);
    }
    int ret_send_key = enc_client_send(fd_keyfile, sd_client); 
    if (ret_send_key < 0){
        perror("CLIENT: ERROR sending keyfile to socket");
        exit(1);
    }

    // Receive Data Transfer: receive processed data from server  -- search for & to find stop signal
    bool stopSignal = false;
    while (!stopSignal) {
        memset(buffer, '\0', sizeof(buffer));
        charsRead = recv(sd_client, buffer, BUFFSIZE - 1, 0);
        if (charsRead < 0){
            perror("CLIENT: ERROR reading from socket");
            exit(1);
        } 
        

        // check for stop Signal
        for (int i = 0; i < strlen(buffer); i++) {
            if (strcmp(&buffer[i], DELIM_STR) == 0) {
                stopSignal = true; 
            } else {
                fflush(stdout);
                printf("%c",buffer[i]);
            }
        }
    }
    // add new line character to output
    printf("\n");
    
    

    close(sd_client);    
    return EXIT_SUCCESS;
}

