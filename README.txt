Russell Feathers
Due Date: 11/29/21
CS 344: Operating Systems
HW5 -- Sockets/ Network Communication

------------------------------
How to run file?
-----------------------------
Sample file run (See testing.sh)
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

------------------------------
How to make file?
-----------------------------
bash compileall.sh

------------------------------
file description
------------------------------
enc_client.c 
int main(int argc, char *argv[]); 
Notes:
@ is used as delimiter signal between different file information
! is used as an error signal between server and client

0 -- take following argument inputs: plaintextFileName keyFileName portNumber
1 -- Check for Invalid Input (too many or too few arguments)
2 -- Set up client socket and Addressing to server based on input portNumber
3 -- Server Connection Validation: Send message to server and only proceed if
        connection is made to ENC_SERVER
4 -- Plain Text and Key File Validation:  Open plaintext and keyfiles, read ea. 
        character and validate characters are capital A through Z or space
5 -- Send Data Transfer: Send string of data size to server in following format:
        @Plain Text File Size@Key Text File Size@
6 -- Send Data Transfer: Send string of data to server in buffer increments using 
        following format: @Plain Text@Key File@
7 -- Receive Data Transfer: Receive data back from server and print out to output
        using following format: @EncryptedText@

------------------------------
enc_client_process.c
int enc_client_process(int fd_plain, int fd_key, int fd_socket) 

4 -- Plain Text and Key File Validation
        function cycles through each chracter in plain text and key file to validate each character is capital A through
        capital Z or Space character.
5 -- Send Data Transfer: Get file size and send string of data size to server in following format:
        @Plain Text File Size@Key Text File Size@  

------------------------------
enc_client_send.c
int enc_client_send(int fd_txt, int sd_client) 

6 -- Send Data Transfer: Send string of data to server in buffer increments. replaces new line 
        character in plaintext file with delimiter key.Checks for errors with writing and restarts 
        where left off. 

------------------------------
enc_server.c
int main(int argc, char *argv[]) 

0 -- take following argument inputs: portNumber
1 -- Check for Invalid Input (too many or too few arguments)
2 -- Set up server socket and Addressing to server based on input portNumber
3 -- Bind socket to input port
4 -- Listen on socket for clients
5 -- Create pool of 5 children processes by forking prior to "running" server
6 -- While(1) to run server continously until signal from command line
7 -- Accept Clients from any addresses
8 -- Server Connection Validation: Receive client name message from client and if ENC_CLIENT 
        connection is made to server
9 -- Plain Text and Key File Size Recieve:  Receive file size from client and allocate
        buffers to accomodate plaintext and keyfile
10 -- Receive Data Transfer: Receive data from client in following format:
        @Plain Text File Size@Key Text File Size@
        If any errors during process, then data transfer tracks indices to update
11 -- Process Data: Encrypt data using enc_server_encrypt
12 -- Send Processed Data back: Send processed data back in following format: 
        @EncryptedText@

------------------------------
enc_server_encrypt.c
char enc_server_encrypt(char plainChar, char keyChar) 

Encrypts plaintext file using keyFile characters by adding key file characters
and taking remainder from division by 27. 

------------------------------
dec_client.c 
int main(int argc, char *argv[]); 

see enc_client.c.
Validates to only work with DEC_SERVER.

------------------------------
dec_server.c
int main(int argc, char *argv[]) 

see enc_server.c.
Calls decryption rather than encryption

------------------------------
dec_server_encrypt.c
char enc_server_encrypt(char plainChar, char keyChar) 

Decrypts encrypted plaintext file using keyFile characters by subtracting
key file from encrypted value and taking remainder from division by 27. 