#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define NO_OF_PHILOSOPHERS 5
#define MAX_SZ 1024

char null_string[MAX_SZ] = "---------\0";

int placeFork(int port_no,char *address);
int takeFork(int port_no,int id,char *address);
void sendMonitor(char *state,char *address,int port_no);

int philosopherProcess(int port_no, int state,char* address,int id){
	//printf("port_no:%d\tstate:%d\taddress:%s\tid:%d\n",port_no,state,address,id);
	int i,count = 0;
	int chop1 = port_no + id;
	int chop2 = port_no + ((id +1) % 5);
	
    char buffer[MAX_SZ];
    	
	if(state==0)
	{
		bzero((char *)buffer,sizeof(buffer));
		for(i=0;i<NO_OF_PHILOSOPHERS;i++){
			if(i == id){
				sprintf(buffer,"%s \t Thinking",buffer);
				continue;
			}
			sprintf(buffer,"%s \t %s",buffer,null_string);
		}
		sendMonitor(buffer,address,port_no);
		
		bzero((char *)buffer,sizeof(buffer));
		for(i=0;i<NO_OF_PHILOSOPHERS;i++){
			if(i == id){
				sprintf(buffer,"%s \t Hungry",buffer);
				continue;
 			}
			sprintf(buffer,"%s \t %s",buffer,null_string);
		}
		sendMonitor(buffer,address,port_no);
	}
	else
	{
   
		int n, len;int num;
		int right = takeFork(chop2,id,address);
		int left = takeFork(chop1,id,address);
		if(right == 1  && left == 1)
		{
			printf("forks %d and %d picked by the philosopher %d\n",chop2,chop1,id);
			count++;
			bzero((char *)buffer,sizeof(buffer));
			for(i=0;i<NO_OF_PHILOSOPHERS;i++){
				if(i == id){
					sprintf(buffer,"%s \t Eating",buffer);
					continue;
				}
				sprintf(buffer,"%s \t %s",buffer,null_string);
			}
			sendMonitor(buffer,address,port_no);
			sleep(90);

			bzero((char *)buffer,sizeof(buffer));
			for(i=0;i<NO_OF_PHILOSOPHERS;i++){
				if(i == id){
					sprintf(buffer,"%s \t Thinking",buffer);
					continue;
				}
				sprintf(buffer,"%s \t %s",buffer,null_string);
			}
			sendMonitor(buffer,address,port_no);
			num = (rand() + 1) % 17;
			placeFork(chop2,address);
			placeFork(chop1,address);
			sleep(num);
		}
		else if(right==1 && left==0)
		{
			printf("\n waiting for fork %d\n",(chop2-port_no));
			
			bzero((char *)buffer,sizeof(buffer));
			for(i=0;i<NO_OF_PHILOSOPHERS;i++){
				if(i == id){
					sprintf(buffer,"%s \t Waiting",buffer);
					continue;
				}
				sprintf(buffer,"%s \t %s",buffer,null_string);
			}
			sendMonitor(buffer,address,port_no);
			num = (rand() + 1) % 10;
			placeFork(chop2,address);
			sleep(num);
		}	
		else if(right==0 && left==1)
		{		
			printf("\n waiting for fork %d\n",(chop1-port_no));
			
			bzero((char *)buffer,sizeof(buffer));
			for(i=0;i<NO_OF_PHILOSOPHERS;i++){
				if(i == id){
					sprintf(buffer,"%s \t Waiting",buffer);
					continue;
				}
				sprintf(buffer,"%s \t %s",buffer,null_string);
			}
			sendMonitor(buffer,address,port_no);
			num = (rand() + 1) % 10;
			placeFork(chop1,address);
			sleep(num);
		}
    	return count;
	}

}

int placeFork(int port_no,char *address)
{

	int count;
  	int client_sockfd;
  	char buffer[MAX_SZ];
  	struct sockaddr_in server;
  	socklen_t addr_size;

  	server.sin_family = AF_INET;
  	server.sin_port = htons(port_no);
	server.sin_addr.s_addr = inet_addr(address);
  	memset(server.sin_zero, '\0', sizeof server.sin_zero);  
  	addr_size = sizeof server;

  	if((client_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
     		perror("socket creation failed\n");
     		exit(1);
  	}

  	if(connect(client_sockfd, (struct sockaddr *) &server, addr_size) < 0) 
	{
		printf("failed the connection: %d \n",port_no);
		sleep(1);
		return 0;
  	}
  	strcpy(buffer,"release");
  	write(client_sockfd, buffer, sizeof(buffer));
   	close(client_sockfd);
  	return count;
}

int takeFork(int port_no,int id,char *address){
	//printf("port_no:%d\tid:%d\taddress:%s\n",port_no,id,address);
  	int count;
  	int client_sockfd;
  	char buffer[MAX_SZ];
  	struct sockaddr_in server;

  	server.sin_family = AF_INET;
  	server.sin_port = htons(port_no);
  	server.sin_addr.s_addr = inet_addr(address);
	memset(server.sin_zero, '\0', sizeof(server.sin_zero));
  	if((client_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
     		perror("socket creation failed");
     		exit(1);
  	}
  	if(connect(client_sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
    	printf("unable to make the connection to the fork - %d\n",port_no);
		return 0;
  	}
	strcpy(buffer,"check");
  	write(client_sockfd,buffer,sizeof(buffer));
    recv(client_sockfd,buffer,sizeof(buffer),0);
	if(strcmp(buffer,"using")==0){
		count = 1;
	}
	else{
		count = 0;
	}
  	close(client_sockfd);

  	return count;
}

void sendMonitor(char *state,char *address,int port_no)
{
  	char buffer[MAX_SZ];
	strcpy(buffer,state);
 	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));

  	int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
  	server.sin_family = AF_INET;
  	server.sin_port = htons(port_no + 5);
	server.sin_addr.s_addr = inet_addr(address);

    sendto(socketfd,buffer,MAX_SZ,0,(struct sockaddr *)&server,sizeof(server));
	printf("%s\n",state);

	close(socketfd);
}


