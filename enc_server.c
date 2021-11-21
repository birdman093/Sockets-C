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
#define BUFFSIZE 256
#define ENC_SERVER_STRING "enc_server"

int main(int argc, char *argv[]) 
{
    // Handle invalid input: too many inputs
    if (argc > 2) {
        printf("Too Many arguments\n");
        return EXIT_FAILURE;
    } else if (argc == 1) {
        printf("Please give me a port!\n");
        return EXIT_FAILURE;
    }

    int sd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (sd_listen == -1) {
        printf("Error creating Socket\n");
        return EXIT_FAILURE;
    }
    struct sockaddr_in sd_address;
    memset((char*) &sd_address, '\0', sizeof(sd_address));
    sd_address.sin_family = AF_INET;
    sd_address.sin_addr.s_addr = INADDR_ANY;
    sd_address.sin_port = htons(atoi(argv[1]));

    int ret_Bind = bind(sd_listen, (struct sockaddr *)&sd_address, sizeof(sd_address));
    if (ret_Bind == -1) {
        perror("Error binding to Port\n");
        exit(1);
    }

    // children listen on same socket for requests
    int ret_Listen = listen(sd_listen, 5);
    if (ret_Listen == -1) {
        printf("Error Listening\n");
        return EXIT_FAILURE;
    }

    // Create pool of 5 child processes that will listen and accept incoming client requests
    pid_t newProcess = -5;
    for (int i = 0; i < MAXCONN; i++) {
        newProcess = fork();
        if (newProcess == -1) {
            printf("Invalid Fork--New Process did not fork\n");
            return EXIT_FAILURE;
        } else if (newProcess == 0) {
            //child process
            break;
        }
    }
    
    //Child Processes to accept incoming requests as server and process data
    if (newProcess == 0) {

        // Accept clients to the server and send to pooled processes
        int connectSocket;
        struct sockaddr_in clientAddress;
        socklen_t clientSize = sizeof(clientAddress);

        // buffer info
        char buffer[BUFFSIZE];
        int charsRead;

        while(1){
            
            // accept new clients, OS to determine which child process will gain access to client socket fd
            connectSocket = accept(sd_listen, (struct sockaddr *)&clientAddress, &clientSize);
            if (connectSocket < 0) {
                perror("ERROR on accept");
                return EXIT_FAILURE;
            }

            // FOR TESTING ONLY: Display client info
            printf("SERVER: Connected to client running at host %d port %d\n", 
                            ntohs(clientAddress.sin_addr.s_addr),
                            ntohs(clientAddress.sin_port));

            // send client server name for client error checking
            charsRead = send(connectSocket, ENC_SERVER_STRING, 11, 0); 
            if (charsRead < 0){
                perror("ERROR writing to socket");
            }

            /*
            // Read the client's message from the socket
            memset(buffer, '\0', BUFFSIZE);
            charsRead = recv(connectSocket, buffer, BUFFSIZE-1, 0); 
            if (charsRead < 0){
                perror("ERROR reading from socket");
            }
            printf("SERVER: Starting Communication with from the client: \"%s\"\n", buffer);
            */

            close(connectSocket);
        }
        close(sd_listen);
    }
    else {
        close(sd_listen);
        pause();
    }

    return EXIT_SUCCESS;
}

