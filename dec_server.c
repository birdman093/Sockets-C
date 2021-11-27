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
#define DEC_CLIENT "dec_client"
#define BUFFSIZE 256
#define DEC_SERVER_STRING "dec_server"
#define ERROR_KEY '!'
#define ERROR_STR "!"
#define DELIM_KEY '@'
#define DELIM_STR "@"
#define ASCII_ZERO 48

char dec_server_decrypt(char, char);

int main(int argc, char *argv[]) 
{
    // Handle invalid input: too many inputs
    if (argc > 2) {
        perror("DEC SERVER: Too Many arguments\n");
        return EXIT_FAILURE;
    } else if (argc == 1) {
        perror("DEC SERVER: Please give me a port!\n");
        return EXIT_FAILURE;
    }

    // Server Socket and Addressing
    int sd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (sd_listen == -1) {
        perror("DEC SERVER: Error creating Socket\n");
        return EXIT_FAILURE;
    }
    struct sockaddr_in sd_address;
    memset((char*) &sd_address, '\0', sizeof(sd_address));
    sd_address.sin_family = AF_INET;
    sd_address.sin_addr.s_addr = INADDR_ANY;
    sd_address.sin_port = htons(atoi(argv[1]));

    // bind socket to input port
    int ret_Bind = bind(sd_listen, (struct sockaddr *)&sd_address, sizeof(sd_address));
    if (ret_Bind == -1) {
        perror("DEC SERVER: Error binding to Port\n");
        return EXIT_FAILURE;
    }

    // all children listen on same socket for requests.  Initialize before forking.
    int ret_Listen = listen(sd_listen, 5);
    if (ret_Listen == -1) {
        perror("DEC SERVER: Error Listening\n");
        return EXIT_FAILURE;
    }

    // Create pool of 5 child processes from the parent process that will listen and accept incoming client requests
    pid_t newProcess = -5;
    for (int i = 0; i < MAXCONN; i++) {
        newProcess = fork();
        if (newProcess == -1) {
            perror("DEC SERVER: Invalid Fork--New Process did not fork\n");
            return EXIT_FAILURE;
        } else if (newProcess == 0) {
            //child process does not fork(), break from loop
            break;
        }
    }
    
    //Child Processes to accept incoming requests as server and process data
    if (newProcess == 0) {

        // Initialize client info and buffer info for receiving from client
        int connectSocket;
        struct sockaddr_in clientAddress;
        socklen_t clientSize = sizeof(clientAddress);
        char buffer[BUFFSIZE];
        int charsRead;
        int charsWritten;
        int andCount;
        int plainSize;
        int keySize;
        char *keyBuffer;
        char *plainBuffer;
        int plainIdx;
        int keyIdx;
        int plainRemain;

        while(1){
            
            // accept new clients, OS to determine which child process will gain access to client socket fd
            connectSocket = accept(sd_listen, (struct sockaddr *)&clientAddress, &clientSize);
            if (connectSocket < 0) {
                perror("DEC SERVER: ERROR accepting client");
                continue;
            }

            // FOR TESTING ONLY: Display client info
            printf("SERVER: Connected to client running at host %d port %d\n", 
                            ntohs(clientAddress.sin_addr.s_addr),
                            ntohs(clientAddress.sin_port));

            // Server Connection Validation Send: send client server name for client error checking
            charsWritten = send(connectSocket, DEC_SERVER_STRING, 11, 0); 
            if (charsWritten < 0){
                perror("SERVER: ERROR writing validation message to socket");
                close(connectSocket);
                continue;
            }

            // Server Connection Validation Receive: close socket if invalid and continue accepting new clients
            memset(buffer, '\0', BUFFSIZE);
            charsRead = recv(connectSocket, buffer, BUFFSIZE-1, 0); 
            if (charsRead < 0 || buffer[0] == ERROR_KEY) {
                perror("SERVER: ERROR receiving validation message from socket");
                close(connectSocket);
                continue;
            }

            // Keyfile/ PlainText File Validation: Receive file sizes from client, allocate buffers
            int error = 0;
            memset(buffer, '\0', BUFFSIZE);
            charsRead = recv(connectSocket, buffer, BUFFSIZE-1, 0);
            if (charsRead < 0) {
                perror("SERVER: error receiving file sizes from socket");
                close(connectSocket);
                continue;
            } 
            
            andCount = 0; plainSize = 0; keySize = 0;
            int buffLength = strlen(buffer);
            int lastIdx = 0;
            for (int i = 0; i < strlen(buffer); i++) {
                if (andCount >= 3) {
                    break;
                } else if (buffer[i] == DELIM_KEY) {
                    andCount++;
                } else if (andCount == 1) {
                    plainSize = plainSize * 10 + (int)buffer[i] - ASCII_ZERO;
                } else if (andCount == 2) {
                    keySize = keySize * 10 + (int)buffer[i] - ASCII_ZERO;
                } else {
                    perror("SERVER: incorrect file format from client");
                    close(connectSocket);
                    error = 1;
                    break;
                }
                lastIdx += 1;
            }
            if (error == 1) {
                continue;
            }
            keyBuffer = calloc(keySize + 3, sizeof(char));
            plainBuffer = calloc(plainSize + 3, sizeof(char));

            // DataTransfer Receive:  Leftovers between receive to be picked up here
            andCount = 0; plainIdx = 0; keyIdx = 0;
            char* ret = NULL;
            int lastPlain = plainIdx;
            int lastKey = keyIdx;
            int lastAnd = andCount;
            if ((ret = strstr(buffer,ERROR_STR)) != NULL) {
                    lastIdx = (int)(ret-&buffer[0]) + 1;
            }
            for (int i = lastIdx; i < buffLength; i++) {
                    printf("%c", buffer[i]);
                    if (buffer[i] == DELIM_KEY) {
                        andCount++;
                    } else if (andCount == 1) {
                        plainBuffer[plainIdx] = buffer[i];
                        plainIdx++; 
                    } else if (andCount == 2) {
                        keyBuffer[keyIdx] = buffer[i];
                        keyIdx++; 
                    } else {
                        perror("SERVER: Not receiving properly formated data from Client");
                        free(keyBuffer);
                        free(plainBuffer);
                        close(connectSocket);
                        error = 1;
                        break;
                    }
            }


            // DataTransfer Receive:  Receive plaintext files and keyfiles with delimiters
            lastPlain = plainIdx;
            lastKey = keyIdx;
            lastAnd = andCount;
            int bufferStart = 0;
            charsRead = 1;
            while (charsRead > 0 && andCount < 3) {

                memset(buffer, '\0', BUFFSIZE);
                charsRead = recv(connectSocket, buffer, BUFFSIZE-1, 0); 
                if (charsRead <= 0) {
                    perror("SERVER: error receiving plaintext and key files from socket\n");
                    //free(keyBuffer);
                    //free(plainBuffer);
                    close(connectSocket);
                    error = 1;
                    break;
                }

                // Check for Error string from client
                if((ret = strstr(buffer,ERROR_STR)) != NULL) {
                    plainIdx = lastPlain;
                    keyIdx = lastKey;
                    andCount = lastAnd;
                    bufferStart = (int)(ret-&buffer[0]) + 1;
                } else {
                    lastPlain = plainIdx;
                    lastKey = keyIdx;
                    lastAnd = andCount;
                    bufferStart = 0;
                }

                for (int i = bufferStart; i < strlen(buffer); i++) {
                    if (buffer[i] == DELIM_KEY) {
                        andCount++;
                    } else if (andCount == 1) {
                        plainBuffer[plainIdx] = buffer[i];
                        plainIdx++; 
                    } else if (andCount == 2) {
                        keyBuffer[keyIdx] = buffer[i];
                        keyIdx++; 
                    } else {
                        perror("SERVER: Not receiving properly formated data from Client");
                        free(keyBuffer);
                        free(plainBuffer);
                        close(connectSocket);
                        error = 1;
                        break;
                    }
                }
            }
            if (error == 1) {
                continue;
            }

            // Process Data: Encryption
            for (int i = 0; i < strlen(plainBuffer); i++) {
                plainBuffer[i] = dec_server_decrypt(plainBuffer[i], keyBuffer[i]);
            }

            //Process Data: Send Processed info back to client, send DELIM_KEY at end
            plainIdx = 0;
            plainRemain = plainSize;
            while (plainRemain > 0) {
                memset(buffer, '\0', BUFFSIZE);
                if (plainRemain > BUFFSIZE-1) {
                    strncpy(buffer, &plainBuffer[plainIdx], BUFFSIZE-1);
                    plainIdx += BUFFSIZE-1;
                    plainRemain -= BUFFSIZE -1;
                    
                } else {
                    strncpy(buffer, &plainBuffer[plainIdx], plainRemain);
                    plainIdx += plainRemain;
                    plainRemain -= plainRemain;
                    
                }
                charsWritten = send(connectSocket, buffer, BUFFSIZE-1, 0);
                if (charsWritten < 0){
                    perror("ERROR writing encryption message to socket");
                    free(keyBuffer);
                    free(plainBuffer);
                    close(connectSocket);
                    continue;
                } 
            }

            charsWritten = send(connectSocket, DELIM_STR, 1, 0);
            if (charsWritten < 0){
                perror("ERROR sending stop message to socket");
                continue;
            } 
            //free(keyBuffer);
            //free(plainBuffer);
            close(connectSocket);

        }
    }
    else {
        close(sd_listen);
        pause();
        return EXIT_SUCCESS;
    }

    close(sd_listen);
    return EXIT_SUCCESS;
}

