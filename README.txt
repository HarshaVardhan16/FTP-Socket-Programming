File Transfer Protocol (FTP) using Client-Server socket programming
These functionalities have been implemented in the the program : 

PUT <filename> : Transfer a file from client to server 
GET <filename> : Transfer a file from server to client 
MGET <extension> (example : MGET .txt): Transfer all files of a given extension from server to client 
MPUT <extension> (example : MPUT .txt): Transfer all files of a given extension from client to server

File overwrite feature has been implemented along with it i.e. if the file is already present at the destination, then the client will be asked whether to overwrite the file or not.

Compiling the code :

On Server’s terminal $ gcc server.c -o server
On Client’s terminal $ gcc client.c -o client

To run the code, First on the Server’s terminal $ ./server <PORT NUMBER>

Then on Client’s terminal run $ ./client <SERVER IP> <PORT NUMBER>

Commands : 
Enter a choice: GET PUT MGET MPUT QUIT

Closing Client and Server
Use command “QUIT server” to quit client program. This will disconnect the client. You can exit the server by simply using Ctrl + C on the server terminal.

