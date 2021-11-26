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
#define MIN_ASCII 65
#define MAX_CHARS 27
#define MAX_ASCII 90
#define ASCII_SPACE 32
#define MAX_ASCII_CHAR 'Z'
#define MIN_ASCII_CHAR 'A'
#define ASCII_CHAR_SPACE ' '
#define ASCII_CHAR_NL '\n'
#define DELIM_KEY '@'
#define DELIM_STR "@"
#define ZERO_ASCII 48

// return size of file, return -1 on error
int enc_client_process(int fd_plain, int fd_key, int fd_socket) 
{

    // get plaintext file size
    int ret_plain;
    struct stat plain;
    fstat(fd_plain, &plain);
    ret_plain = plain.st_size;

    // get key file size
    int ret_key;
    struct stat key;
    fstat(fd_key, &key);
    ret_key = key.st_size;

    if (ret_key < ret_plain) {
        return -1;
    }

    // sort through plain text characters for illegal characters.  
    char buffChk[BUFFSIZE];
    ssize_t nread2;
    int byteCounter = 0; 
    while ((nread2 = read(fd_plain, buffChk, BUFFSIZE-1)) > 0) {
        for (int i = 0; i < strlen(buffChk); i++) {
            byteCounter ++;
            if (buffChk[i] > MAX_ASCII_CHAR || (buffChk[i] < MIN_ASCII_CHAR && buffChk[i] != ASCII_CHAR_SPACE)) {
                //if at end of text file and hit newline that is acceptable
                if (buffChk[i] != ASCII_CHAR_NL && byteCounter != ret_plain-1) {
                    return -1; 
                }
            }
        }
    }
    lseek(fd_plain,0,SEEK_SET);

    // sort through key text characters for illegal characters
    byteCounter = 0;
    while ((nread2 = read(fd_key, buffChk, BUFFSIZE-1)) > 0) {
        for (int i = 0; i < strlen(buffChk); i++) {
            byteCounter ++;
            if (buffChk[i] > MAX_ASCII_CHAR || (buffChk[i] < MIN_ASCII_CHAR && buffChk[i] != ASCII_CHAR_SPACE)) {
                //if at end of text file and hit newline that is acceptable
                if (buffChk[i] != ASCII_CHAR_NL && byteCounter != ret_key-1) {
                    perror("INADEQUATE TEXT FILE");
                    return -1; 
                }
            }
        }
    }
    lseek(fd_key,0,SEEK_SET);

    // send information to server
    memset(buffChk, '\0', BUFFSIZE);
    strcpy(&buffChk[0], DELIM_STR);
    int len = 1;
    sprintf(&buffChk[len], "%i", ret_plain);
    len = strlen(buffChk);
    strcpy(&buffChk[len], DELIM_STR);
    len = strlen(buffChk);
    sprintf(&buffChk[len], "%i",ret_key);
    len = strlen(buffChk);
    strcpy(&buffChk[len], DELIM_STR);

    int charsWritten = send(fd_socket, buffChk, strlen(buffChk), 0);
    if (charsWritten < 0) {
        perror("CLIENT: ERROR writing to socket");
        return -1;
    }

    return 0;
}




