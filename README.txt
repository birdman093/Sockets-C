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

            /*
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
            */