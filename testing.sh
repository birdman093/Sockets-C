#!/bin/bash
#pkill enc_server
#reset
portNum=58930
portNum2=58931
keyLength=70000 
keygen $keyLength > keyfile
enc_server $portNum &
dec_server $portNum2 &
enc_client plaintext1 keyfile $portNum > enctxt1
enc_client plaintext2 keyfile $portNum > enctxt2
enc_client plaintext3 keyfile $portNum > enctxt3
enc_client plaintext4 keyfile $portNum > enctxt4
enc_client plaintext5 keyfile $portNum > enctxt5
dec_client enctxt1 keyfile $portNum2 > dectxt1
dec_client enctxt2 keyfile $portNum2 > dectxt2
dec_client enctxt3 keyfile $portNum2 > dectxt3
dec_client enctxt4 keyfile $portNum2 > dectxt4

#gdb enc_client
#break 110
#run plaintext3 keyfile 58926