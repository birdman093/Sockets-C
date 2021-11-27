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
#define MAX_ASCII 90
#define MAX_CHARS 27
#define ASCII_SPACE 32
#define MAX_ASCII_CHAR 'Z'
#define MIN_ASCII_CHAR 'A'
#define ASCII_CHAR_SPACE ' '
#define ASCII_CHAR_NL '\n'

// return size of file, return -1 on error
char dec_server_decrypt(char plainChar, char keyChar) 
{
    // convert input Char to raw number from 0 to 26
    int inpChar;
    if ((int)plainChar == ASCII_SPACE) {
        // 26
        inpChar = 26;
    } else {
        // 0 to 25
        inpChar = (int)plainChar - MIN_ASCII;
    }

    // convert key Char to raw number from 0 to 26
    int inpKey;
    if ((int)keyChar == ASCII_SPACE) {
        // 26
        inpKey = 26;
    } else {
        // 0 to 25
        inpKey = (int)keyChar - MIN_ASCII;
    }

    // decrypt to number from 0 to 26 by subtracting key
    // if value is less than 0 than add 27 to value
    inpChar -= inpKey;
    if (inpChar < 0) {
        inpChar += MAX_CHARS;
    }
    inpChar = inpChar % MAX_CHARS;

    // convert to ASCII character
    if (inpChar == 26) {
        inpChar = ASCII_SPACE;
    } else {
        inpChar += MIN_ASCII;
    }

    return (char)inpChar;
}


