#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet.h>
#include <netdb.h>
#include <sys/socket/h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


#define MAXLINES 50
#define MAXCHAR 1001

int main(int argc, char *argv[]) 
{
    // Handle invalid input: too many inputs
    if (argc >= 2) {
        printf("Too Many arguments\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}