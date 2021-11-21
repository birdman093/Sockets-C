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

int enc_client_process(int, int); 
int enc_client_send(int, int);

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

    //Error Checking:  check server name
    char buffer[BUFFSIZE];
    memset(buffer, '\0', sizeof(buffer));
    int charsRead;
    charsRead = recv(sd_client, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        perror("CLIENT: ERROR reading from socket");
    }
    if (strcmp(buffer, ENC_SERVER_STRING) != 0) {
        perror("CLIENT: INVALID SERVER");
        exit(2);
    }

    // Open plaintext and keyfile, check for any errors
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

    // Check plaintext and keyfile for any invalid characters or if keyfile is not long enough
    int ret_process = enc_client_process(fd_plaintext, fd_keyfile);
    if (ret_process < 0) {
        perror("CLIENT: ERROR, processing plaintext and keytext files");
        close(fd_plaintext);
        close(fd_keyfile);
        exit(1);
    }

    // Send key and plaintext to server, send key in between and at end to signify start and end
    int ret_send_pt = enc_client_send(int fd_plaintext, int sd_client); 
    if (ret_send_pt < 0){
        exit(1);
    }
 
    int ret_send_key = enc_client_send(int fd_keyfile, int sd_client); 
    if (ret_send_key < 0){
        exit(1);
    }

    // receive processed data from server  -- search for & to find stop signal
    memset(buffer, '\0', sizeof(buffer));
    int charsRead;
    charsRead = recv(sd_client, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        perror("CLIENT: ERROR reading from socket");
        exit(1);
    }

    close(sd_client);    
    return EXIT_SUCCESS;
}

