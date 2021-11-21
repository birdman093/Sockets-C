#!/bin/bash
portNum=58927
keyLength=100 
keygen $keyLength > keyfile
enc_server $portNum &
enc_client plaintext3 keyfile $portNum
