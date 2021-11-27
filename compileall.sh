#!/bin/bash
gcc -g -std=gnu99 -Wall -o enc_server enc_server.c enc_server_encrypt.c
gcc -g -std=gnu99 -Wall -o enc_client enc_client.c enc_client_process.c enc_client_send.c
gcc -g -std=gnu99 -Wall -o dec_server dec_server.c dec_server_decrypt.c
gcc -g -std=gnu99 -Wall -o dec_client dec_client.c enc_client_process.c enc_client_send.c
gcc -g -std=gnu99 -Wall -o keygen keygen.c