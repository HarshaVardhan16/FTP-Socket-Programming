#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include<time.h>
#include <ifaddrs.h>
#include<fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <dirent.h>

#define sa struct sockaddr_in
#define  maxlen 100
#define listen_queue 50
#define u_int unsigned int
#define server_path "./server_files/"


void dataConnectionSend(sa clientAddress,int client_socket, unsigned long int port, char* filename){
	sa clientAddress2;

	
	int client_data_socket= socket(AF_INET,SOCK_STREAM,0);
	if(client_data_socket==-1){
		printf("Data Socket cannot be opened\n");
		return ;
	}

 	bzero(&clientAddress2, sizeof(clientAddress2));

	clientAddress2.sin_family = AF_INET;
	clientAddress2.sin_addr.s_addr = clientAddress.sin_addr.s_addr;
	clientAddress2.sin_port = port;
	
	
	int status_connect = connect(client_data_socket,(struct sockaddr*) &clientAddress2,sizeof(clientAddress2));
	
	if(status_connect<0){
		printf("Connection Error\n");
		return ;
	}
	//printf("Connection Established\n");
	int filehandle,size;
	char file[maxlen];
	struct stat object;
	strcpy(file,server_path);
	strcat(file,filename);
	stat(file,&object);
	filehandle = open(file,O_RDONLY);
	size = object.st_size;
	char data[10];
	memset(data,0,sizeof(data));
	if(filehandle == -1){
		size = 0;
		sprintf(data,"%d",size);
		
		send(client_socket,data,10,0);
	}
	else{
		int ow = 0;
		sprintf(data,"%d",size);
		
		send(client_socket,data,10,0);
		recv(client_socket,&ow,sizeof(int),0);
		if(ow == 1){
			sendfile(client_data_socket,filehandle,NULL,size);
		}
	}
	close(client_data_socket);
}
void dataConnectionSend_mget(int client_data_socket,sa clientAddress,int client_socket, unsigned long int port, char* filename){
	int filehandle,size;
	char file[maxlen];
	struct stat object;
	strcpy(file,server_path);
	strcat(file,filename);
	stat(file,&object);
	filehandle = open(file,O_RDONLY);
	size = object.st_size;
	
	char data[10];
	memset(data,0,sizeof(data));
	if(filehandle == -1){
		size = 0;
		sprintf(data,"%d",size);
		
		send(client_socket,data,10,0);
	}
	else{
		int ow = 0;
		sprintf(data,"%d",size);
		
		send(client_socket,data,10,0);
		recv(client_socket,&ow,sizeof(int),0);
		if(ow == 1){
			sendfile(client_data_socket,filehandle,NULL,size);
		}
	}

}
void dataConnectionReceive(sa clientAddress,int client_socket, unsigned long int port, char* filename){
	sa clientAddress2;

	
	int client_data_socket= socket(AF_INET,SOCK_STREAM,0);
	if(client_data_socket==-1){
		printf("Data Socket cannot be opened\n");
		return ;
	}

 	bzero(&clientAddress2, sizeof(clientAddress2));

	clientAddress2.sin_family = AF_INET;
	clientAddress2.sin_addr.s_addr = clientAddress.sin_addr.s_addr;
	clientAddress2.sin_port = port;

	
	int status_connect = connect(client_data_socket,(struct sockaddr*) &clientAddress2,sizeof(clientAddress2));

	if(status_connect<0){
		printf("Connection Error\n");
		return ;
	}
	//printf("Connection Established\n");
	int size,status,filehandle;
	char *receiveFile;
	char file[maxlen];
	strcpy(file,server_path);
	strcat(file,filename);
	int exists, overwrite;
	if( access( file, F_OK ) != -1){
        exists = 1;
        send(client_socket,&exists,sizeof(int),0);              
    }else{
        exists = 0;
        send(client_socket,&exists,sizeof(int),0);            
    }
    recv(client_socket,&overwrite,sizeof(int),0);
	char data[10];
	memset(data,0,sizeof(data));
    if(overwrite == 1)
    {
    	if(exists == 1)
    		filehandle = open(file, O_WRONLY | O_CREAT | O_TRUNC, 644);     
    	else
    		filehandle = open(file, O_CREAT | O_EXCL | O_WRONLY, 0666);     

    	
		recv(client_socket,data,10,0);
		size = atoi(data);

        receiveFile = (char*)malloc(size*sizeof(char));
        recv(client_data_socket, receiveFile, size, 0);			
        status = write(filehandle, receiveFile, size);	
        close(filehandle);
        send(client_socket, &status, sizeof(int), 0);		
    }

    close(client_data_socket);
}

void getMyIP(char ip[]){  // Function to get the IP.
	/* Variables to Store the IP address of the Machine on which SERVER is running. */
	struct ifaddrs * ipAddrstr=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ipAddrstr);

    for (ifa = ipAddrstr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if(strncmp(ifa->ifa_name,"eth0",3)==0){
            	strcpy(ip,addressBuffer);
            }
        }
    }
}

int main(int argc, char *argv[])
{
	sa serverAddress;

	int server_socket = socket(AF_INET,SOCK_STREAM,0);
	if(server_socket == -1){ 
		printf("Error in Opening the Listening Port.\n");
		return 0;
	}
	char ip[20];
    getMyIP(ip);

    unsigned long int IP = inet_addr(ip);   
    memset(&serverAddress,0, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = IP;
	serverAddress.sin_port = atoi(argv[1]);

	int status_bind = bind(server_socket,(struct sockaddr*) &serverAddress,sizeof(struct sockaddr_in));

	if(status_bind == -1){
		printf("Socket Bind Error\n");
		return 0;
	}

	printf("Server started on IP: < %s > at Port: < %s >\n",ip,argv[1]);

	int status_listen = listen(server_socket,listen_queue);
	if(status_listen == -1){
		printf("Listening error at the PORT\n");
		return 0;
	}

	sa clientAddress;
	char buffer[maxlen],command[5], filename[20];
	unsigned long int PORT = 0;
	int filehandle;
	char socket_buffer[maxlen];
	memset(buffer,0,maxlen);
	memset(socket_buffer,0,maxlen);
	socklen_t length = sizeof(sa);
	printf("server waiting to accept\n");
	int client_socket = accept(server_socket,(struct sockaddr*) &clientAddress,&length);


	if (client_socket < 0){
		printf("Error in accepting socket\n");
   	}
	printf("accepted\n");
	while(1){

		
		recv(client_socket,filename,20,0);
		printf("Received file name : %s\n",filename);
		recv(client_socket,buffer,maxlen,0);
		
		int i=0 ,j;
		int m = 0;
		for(i = 0; buffer[i]!=':';i++){
			command[m++] = buffer[i];
		}
		printf("Received command : %s\n",command );
		int k = 0;
		for(j = i+1;buffer[j]!='#';j++){
			socket_buffer[k++] = buffer[j];
		}
		//printf("New data connection port : %s\n",socket_buffer);
		
		if(!strcmp(command,"GET")){

			//recv(client_socket,socket_buffer,sizeof(socket_buffer),0);
			
			unsigned long int PORT = atoi(socket_buffer);
			dataConnectionSend(clientAddress,client_socket,PORT,filename);
	  		memset(buffer,0,maxlen);
	  		memset(socket_buffer,0,maxlen);
		}
		else if(!strcmp(command,"PUT")){
			unsigned long int PORT = atoi(socket_buffer);
			dataConnectionReceive(clientAddress,client_socket,PORT,filename);
			memset(buffer,0,maxlen);
			memset(socket_buffer,0,maxlen);
		}
		else if(!strcmp(command,"MGET")){
			unsigned long int PORT = atoi(socket_buffer);
			sa clientAddress2;

			
			int client_data_socket= socket(AF_INET,SOCK_STREAM,0);
			if(client_data_socket==-1){
				printf("Data Socket cannot be opened\n");
			}

		 	bzero(&clientAddress2, sizeof(clientAddress2));

			clientAddress2.sin_family = AF_INET;
			clientAddress2.sin_addr.s_addr = clientAddress.sin_addr.s_addr;
			clientAddress2.sin_port = PORT;
			
			
			int status_connect = connect(client_data_socket,(struct sockaddr*) &clientAddress2,sizeof(clientAddress2));
			
			if(status_connect<0){
				printf("Connection Error\n");
			}
			char *fname;
	        DIR *d;
	        struct dirent *dir;
	        d = opendir(server_path);
	        int ready=1;
			char newname[20];
			while ((dir = readdir(d)) != NULL){
				fname=dir->d_name;
				memset(newname,0,sizeof(newname));
	            strcpy(newname,fname);
				char* fextension = strrchr(newname, '.');
				if(fextension == NULL) continue;
				if (strcmp(filename,fextension)==0){
					printf("Sending %s file\n",newname);
	                send(client_socket, &ready, sizeof(int), 0);
	                send(client_socket,newname,20,0);
					dataConnectionSend_mget(client_data_socket,clientAddress,client_socket,PORT,newname);
				}
			}
			ready = 0;
			send(client_socket,&ready,sizeof(int),0);
			closedir(d);
			memset(buffer,0,sizeof(buffer));
			close(client_data_socket);
		}
		 else if(!strcmp(command, "QUIT"))
	        {
	            int status = 1;
	            send(client_socket, &i, sizeof(int), 0);           
	            break;
	        }

	}
	return 0;
}
