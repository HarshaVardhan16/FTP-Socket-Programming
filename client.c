#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#define path "./client_files/"

void getMyIP(char ip[]){  // Function to get the IP.
	/* Variables to Store the IP address of the Machine on which Client is running. */
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
            if(strncmp(ifa->ifa_name,"wlp",3)==0){
            	strcpy(ip,addressBuffer);
            }
        }
    }
}
void data_connection_send(unsigned long int dataPort,int sockfd,char* filename){
    struct sockaddr_in clientAddr;
    int clientfd;
    clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(clientfd < 0){
        printf("Error in creating socket for data connection\n");
        return;
    }
    memset(&clientAddr,0,sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_port = dataPort;

    int binding = bind(clientfd,(struct sockaddr*) &clientAddr,sizeof(clientAddr));
    if(binding < 0){
        printf("Error in binding in PUT\n");
        return;
    }
    int lis = listen(clientfd,20);
    if(lis < 0){
        printf("Error in listening in PUT\n");
        return;
    }
    struct sockaddr_in serverAddr;
    socklen_t l = sizeof(serverAddr);
    int acc = accept(clientfd,(struct sockaddr*) &serverAddr,&l);
	printf("Accepted\n");
    int already_exist = 0,overwrite = 1,filehandle;
    char filename_path[50];
    strcpy(filename_path,path);
    strcat(filename_path,filename);
    filehandle = open(filename_path,O_RDONLY);

    recv(sockfd,&already_exist,sizeof(int),0);

    if(already_exist){
        printf("%s file already exists in server \n press 1 for OVERWRITE \n press 0 for NO OVERWRITE\n",filename);
        l:scanf("%d",&overwrite);
        if(overwrite != 0 && overwrite != 1){
            printf("Invalid type 0 or 1\n");
            goto l;
        }
    }
    send(sockfd,&overwrite,sizeof(int),0);
	char data[10];
	memset(data,0,sizeof(data));
    if(overwrite == 1){

        struct stat obj;
        int status,size;
        stat(filename_path,&obj);
        size = obj.st_size;
		sprintf(data,"%d",size);
		send(sockfd,data,10,0);
        //send(sockfd,&size,sizeof(int),0);

        sendfile(acc,filehandle,NULL,size);
        recv(sockfd,&status,sizeof(int),0);
        if(status){
            printf("File successful PUT in the server\n");
        }
        else printf("File not PUT successfully in the server\n");
    }
    close(clientfd);
    close(acc);
}

void data_connection_receive_mget(int acc,unsigned long int dataPort,int sockfd,char* filename){

    

    int size,filehandle,status;
    int already_exist = 0,overwrite = 1;
    char filename_path[50];
    strcpy(filename_path,path);
    strcat(filename_path,filename);

    char data[10];
    memset(data,0,sizeof(data));
    //recv(sockfd,&size,sizeof(int),0);
    recv(sockfd,data,10,0);
    size = atoi(data);
   

    if(size){
        if(access(filename_path, F_OK) != -1){
            already_exist = 1;
            printf("%s file already exists in server \n press 1 for OVERWRITE \n press 0 for NO OVERWRITE\n",filename);
            l:scanf("%d",&overwrite);
            if(overwrite != 0 && overwrite != 1){
                    printf("Invalid type 0 or 1");
                    goto l;
            }
        }
        send(sockfd,&overwrite,sizeof(int),0);

        if(overwrite == 1){
            if(already_exist==1)
                filehandle = open(filename_path, O_WRONLY | O_CREAT | O_TRUNC, 644);
            else
                filehandle = open(filename_path, O_CREAT | O_EXCL | O_WRONLY, 0666);

            char *receive_file = (char*)malloc(size*sizeof(char));
            recv(acc, receive_file, size, 0);
            status=write(filehandle, receive_file, size);
            close(filehandle);
            if(status){
                printf("File successful GET in the server\n");
            }
            else printf("File not GET successfully in the server\n");
        }
    }
    else{
        printf("No such file exists in server\n");
    }
    return;
}

void data_connection_receive(unsigned long int dataPort,int sockfd,char* filename){

	struct sockaddr_in clientAddr;
    int clientfd;
    clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(clientfd < 0){
        printf("Error in creating socket for data connection\n");
        return;
    }
    memset(&clientAddr,0,sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_port = dataPort;

    int binding = bind(clientfd,(struct sockaddr*) &clientAddr,sizeof(clientAddr));
    if(binding < 0){
        printf("Error in binding in PUT\n");
        return;
    }
    int lis = listen(clientfd,20);
    if(lis < 0){
        printf("Error in listening in PUT\n");
        return;
    }
    struct sockaddr_in serverAddr;
    socklen_t l = sizeof(serverAddr);
    int acc = accept(clientfd,(struct sockaddr*) &serverAddr,&l);

    int size,filehandle,status;
    int already_exist = 0,overwrite = 1;
    char filename_path[50];
    strcpy(filename_path,path);
    strcat(filename_path,filename);

	char data[10];
	memset(data,0,sizeof(data));
    //recv(sockfd,&size,sizeof(int),0);
	recv(sockfd,data,10,0);
	size = atoi(data);
	

    if(size){
        if(access(filename_path, F_OK) != -1){
            already_exist = 1;
            printf("%s file already exists in server \n press 1 for OVERWRITE \n press 0 for NO OVERWRITE\n",filename);
            l:scanf("%d",&overwrite);
            if(overwrite != 0 && overwrite != 1){
                    printf("Invalid type 0 or 1");
                    goto l;
            }
        }
        send(sockfd,&overwrite,sizeof(int),0);

        if(overwrite == 1){
            if(already_exist==1)
        		filehandle = open(filename_path, O_WRONLY | O_CREAT | O_TRUNC, 644);
        	else
        		filehandle = open(filename_path, O_CREAT | O_EXCL | O_WRONLY, 0666);

            char *receive_file = (char*)malloc(size*sizeof(char));
            recv(acc, receive_file, size, 0);
            status=write(filehandle, receive_file, size);
            close(filehandle);
            if(status){
                printf("File successful GET in the server\n");
            }
            else printf("File not GET successfully in the server\n");
        }
    }
    else{
        printf("No such file exists in server\n");
    }
    close(clientfd);
    close(acc);
    return;
}

int main(int argc,char *argv[]){
    int sockfd;
	time_t t;
	srand((unsigned) time(&t));
    struct sockaddr_in serverAddr;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        printf("Error in creating socket for control connection\n");
        return 0;
    }
    memset(&serverAddr,0,sizeof(serverAddr));

    unsigned long int ip = inet_addr(argv[1]);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ip;
    serverAddr.sin_port = atoi(argv[2]);
    int conn = connect(sockfd,(struct sockaddr*) &serverAddr,sizeof(serverAddr));
    if(conn < 0){
        printf("error in connection\n");
        return 0;
    }

    while(1){
        char filename[50],comm[20],filename_path[50];
        printf("Enter the choice: command <filename>\n");
        scanf("%s %s",comm,filename);

        char buffer_comm[100],buffer_soc[20];
        memset(buffer_comm,0,sizeof(buffer_comm));
		memset(buffer_soc,0,sizeof(buffer_soc));
        strcpy(buffer_comm,comm);
        strcpy(filename_path,path);
        strcat(filename_path,filename);
		char newname[20];
		memset(newname,0,sizeof(newname));
        if(strcmp(comm,"PUT") == 0){
            if(access(filename_path,F_OK) == -1){
                printf("%s does not exist in client side\n",filename);
            }

			send(sockfd,filename,20,0);
            unsigned long int dataPort = rand()%1000;
			strcat(buffer_comm,":");
			dataPort = dataPort + (long int)10000;

            sprintf(buffer_soc,"%ld",dataPort);

			strcat(buffer_comm,buffer_soc);
			strcat(buffer_comm,"#");
            send(sockfd,buffer_comm,sizeof(buffer_comm),0);
            data_connection_send(dataPort,sockfd,filename);
            printf("File PUT successful\n");
        }
        else if(strcmp(comm,"GET") == 0){


			send(sockfd,filename,20,0);
            unsigned long int dataPort = rand()%1000;
			strcat(buffer_comm,":");
			dataPort = dataPort + (long int)10000;

            sprintf(buffer_soc,"%ld",dataPort);

			strcat(buffer_comm,buffer_soc);
			strcat(buffer_comm,"#");
            send(sockfd,buffer_comm,sizeof(buffer_comm),0);
			//send(sockfd,&dataPort,sizeof(dataPort),0);

            data_connection_receive(dataPort,sockfd,filename);

            
        }

        else if(strcmp(comm,"MPUT") == 0){
			DIR *d;
			struct dirent *dir;
			d = opendir(path);
			while((dir = readdir(d)) != NULL){
				char *fname = dir -> d_name;
				strcpy(newname,fname);
				//printf("%s\n",newname);
				char* fextension = strrchr(newname,'.');
				//printf("%s this is extension\n",fextension);
				if(fextension == NULL) continue;
				else if(!strcmp(filename,fextension)){
					send(sockfd,newname,20,0);
		            unsigned long int dataPort = rand()%1000;
					memset(buffer_comm,0,sizeof(buffer_comm));
					memset(buffer_soc,0,sizeof(buffer_soc));
					strcpy(buffer_comm,"PUT");
					strcat(buffer_comm,":");
					dataPort = dataPort + (long int)10000;
					sprintf(buffer_soc,"%ld",dataPort);

					strcat(buffer_comm,buffer_soc);
					strcat(buffer_comm,"#");
		            send(sockfd,buffer_comm,sizeof(buffer_comm),0);
		            data_connection_send(dataPort,sockfd,newname);
		          
				}
			}
			closedir(d);
        }
        else if(strcmp(comm,"MGET") == 0){
			send(sockfd,filename,20,0);
            unsigned long int dataPort = rand()%1000;
            strcat(buffer_comm,":");
            dataPort = dataPort + (long int)10000;

            sprintf(buffer_soc,"%ld",dataPort);

            strcat(buffer_comm,buffer_soc);
            strcat(buffer_comm,"#");
            send(sockfd,buffer_comm,sizeof(buffer_comm),0);
			int ready ;
			char newname[20];
            memset(buffer_comm,0,sizeof(buffer_comm));
            struct sockaddr_in clientAddr;
            int clientfd;
            clientfd = socket(AF_INET,SOCK_STREAM,0);
            if(clientfd < 0){
                printf("Error in creating socket for data connection\n");
            }
            memset(&clientAddr,0,sizeof(clientAddr));
            clientAddr.sin_family = AF_INET;
            clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            clientAddr.sin_port = dataPort;

            int binding = bind(clientfd,(struct sockaddr*) &clientAddr,sizeof(clientAddr));
            if(binding < 0){
                printf("Error in binding in PUT\n");
            }
            int lis = listen(clientfd,20);
            if(lis < 0){
                printf("Error in listening in PUT\n");
            }
            struct sockaddr_in serverAddr;
            socklen_t l = sizeof(serverAddr);
            int acc = accept(clientfd,(struct sockaddr*) &serverAddr,&l);

			while(1){
                
				recv(sockfd,&ready,sizeof(int),0);
               
				if(ready == 0) break;
				memset(newname,0,sizeof(newname));
				recv(sockfd,newname,20,0);
				data_connection_receive_mget(acc,dataPort,sockfd,newname);

			}
            close(clientfd);
            close(acc);
			printf("File MGET successful\n");
        }
        else if(strcmp(comm,"QUIT")==0)
            {
                // strcpy(buffer, "quit");
                send(sockfd,filename,20,0);
                unsigned long int dataPort = rand()%1000;
                strcat(buffer_comm,":");
                dataPort = dataPort + (long int)10000;

                sprintf(buffer_soc,"%ld",dataPort);

                strcat(buffer_comm,buffer_soc);
                strcat(buffer_comm,"#");
                send(sockfd,buffer_comm,sizeof(buffer_comm),0);
                int status = 0;                
                recv(sockfd, &status, 100, 0);                  
                if(status)
                {
                    printf("Quitting..\n");
                    exit(0);
                }
                printf("Server failed to close connection\n");      
            }
        else{
            printf("command should be in form of GET,PUT,MGET,MPUT");
        }
    }

}
