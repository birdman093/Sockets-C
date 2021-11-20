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
//#define MAXHOSTCHAR 1024



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
        printf("Error binding to Port\n");
        return EXIT_FAILURE;
    }

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

            // get IP address from client and store in IP struct
            struct in_addr ipAdd;
            ipAdd.s_addr = ntohs(clientAddress.sin_addr.s_addr);  //convert to host byte order and place in IP address
            char * clientDotted = calloc(strlen(inet_ntoa(ipAdd))+1, sizeof(char));
            strcpy(clientDotted, inet_ntoa(ipAdd));
            size_t ipSize = sizeof(ipAdd);
            
            // check connection is enc_client by getting client name using gethostbyaddr()
            struct hostent *hostptr; 
            hostptr = gethostbyaddr(&ipAdd, ipSize, AF_INET);
            printf("host Name: %s\n", hostptr->h_name);
            if (strcmp(hostptr->h_name, ENC_CLIENT)!=0) {
                perror("ERROR ON HOST NAME");
                close(connectSocket);
                continue;
            }

            // Read the client's message from the socket
            memset(buffer, '\0', BUFFSIZE);
            charsRead = recv(connectSocket, buffer, BUFFSIZE-1, 0); 
            if (charsRead < 0){
                perror("ERROR reading from socket");
            }
            printf("SERVER: I received this from the client: \"%s\"\n", buffer);

            // process data

            // send data back
            charsRead = send(connectSocket, "I am the server, and I got your message", 39, 0); 
            if (charsRead < 0){
                perror("ERROR writing to socket");
            }

            close(connectSocket);
        }
    }
    else {
        pause();
    }

    return EXIT_SUCCESS;
}

