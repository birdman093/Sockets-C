#!/bin/bash
#pkill enc_server
#reset
portNum=58927
keyLength=70000 
keygen $keyLength > keyfile
enc_server $portNum &
enc_client plaintext1 keyfile $portNum > enctxt1
#enc_client plaintext2 keyfile $portNum > enctxt2
#enc_client plaintext3 keyfile $portNum > enctxt3
#enc_client plaintext4 keyfile $portNum > enctxt4
#enc_client plaintext5 keyfile $portNum > enctxt5
#gdb enc_client
#break 110
#run plaintext3 keyfile 58926