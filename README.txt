Russell Feathers
Due Date: 11/29/21
CS 344: Operating Systems
HW5 -- Sockets

------------------------------
How to run file?
-----------------------------


------------------------------
How to make file?
-----------------------------


------------------------------
file description
------------------------------
main.c 
int main(int argc, char *argv[]); 
Initializes buffer 1, 2, and 3 parameters, mutext parameters, and conditional parameters as well as initializing 
inputStdIn, lineSeperator, plusSign, and output threads. Catches the end of all three threads as well.

------------------------------


    /*
    char hostname[MAXHOSTCHAR];
    hostname[MAXHOSTCHAR-1] = '\0';
    struct hostent* hostptr;

    int ret_host = gethostname(hostname, MAXHOSTCHAR-1);
    if (ret_host == -1) {
        printf("Error getting hostname\n");
        return EXIT_FAILURE;
    }
    hostptr = gethostbyname(hostname);

    printf("Hostname: %s\n", hostname);
    printf("h_name: %s\n", hostptr->h_name);

    struct in_addr ipAdd;
    ipAdd.s_addr = *(unsigned int*)hostptr->h_addr_list[0])
    */

        // set signals of processes to wake up to signals
    sigset_t p_Signal_Set;
    sigemptyset(&p_Signal_Set);
    sigaddset(&p_Signal_Set, SIGUSR1);

    struct sigaction pUSR1;
    pUSR1.sa_handler = handle_USR1;
    sigfillset(&pUSR1.sa_mask);
    pUSR1.sa_flags = 0;

    sigaction(SIGUSR1, &pUSR1, NULL);

    int socketArr[5]; 

void handle_USR1(int signo) {
    // handler for pooled processes
    pause();
}