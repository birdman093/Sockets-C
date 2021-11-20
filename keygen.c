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

#define MIN_ASCII 65
#define MAX_CHARS 27
#define MAX_ASCII 91
#define ASCII_SPACE 32

int main(int argc, char *argv[]) 
{
    // Handle invalid input: too many or too few arguments
    if (argc > 2) {
        printf("Too Many arguments\n");
        return EXIT_FAILURE;
    } else if (argc == 1) {
        printf("Please give me a keylength!\n");
        return EXIT_FAILURE;
    }

    // Loop through keylength and print a character by calculating random value between 
    // MIN_ASCII and MAX_ASCII.  MAX_ASCII is not a valid character, so convert to ASCII_SPACE
    int keyLength = atoi(argv[1]);
    char currChar;
    int randoNum;
    for (int i = 0; i < keyLength; i++) {
        randoNum = rand() % MAX_CHARS;
        randoNum += MIN_ASCII;
        if (randoNum == MAX_ASCII) {
            randoNum = ASCII_SPACE;
        }
        currChar = (char)randoNum;
        printf("%c", currChar);
    }

    return EXIT_SUCCESS;
}

