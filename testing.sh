#!/bin/bash
#pkill enc_server
#reset
portNum=58927
keyLength=100 
keygen $keyLength > keyfile
enc_server $portNum &
enc_client plaintext3 keyfile $portNum
#gdb enc_client
#break 110
#run plaintext3 keyfile 58926