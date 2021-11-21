#!/bin/bash
gcc -std=gnu99 -Wall -o enc_server enc_server.c
gcc -std=gnu99 -Wall -o enc_client enc_client.c enc_client_process.c
#gcc -std=gnu99 -Wall -o dec_server dec_server.c
#gcc -std=gnu99 -Wall -o dec_client dec_client.c
gcc -std=gnu99 -Wall -o keygen keygen.c