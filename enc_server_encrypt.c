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

// return size of file, return -1 on error
char enc_server_encrypt(char plainChar, char keyChar) 
{
    char buffConv;

    // convert to raw number from 0 to 27
    int inpChar;
    if (plainChar == ASCII_SPACE) {
        inpChar = MAX_ASCII - MIN_ASCII;
    } else {
        inpChar = (int)plainChar - MIN_ASCII;
    }

    // encrypt to number from 0 to 27
    inpChar += (int)keyChar;
    inpChar = inpChar % MAX_CHARS;

    // convert to ASCII character
    inpChar += MIN_ASCII;
    if (inpChar > MAX_ASCII) {
        inpChar = ASCII_SPACE;
    }
    buffConv = (char)inpChar;
    return buffConv;
}


