#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SPOONS 5
#define MAX_SZ 255

int chopstick(int forkid,int port){
   	bool flag = false;
   	int socketfd, connection, clilen;
  	struct sockaddr_in client, server;
  	char buffer[MAX_SZ];
  	int len;
  	int option=1;

  	if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     		perror("creating socket failed");
     		exit(1);
  	}
  	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  	
  	bzero((char *) &server, sizeof(server));
  	server.sin_family = AF_INET;
  	server.sin_addr.s_addr = htonl(INADDR_ANY);
  	server.sin_port = htons(port);
  
  	if(bind(socketfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
     		perror("unable to bind the server");
     		exit(1);
  	}
	printf("placed the fork %d on the table\n",forkid);
	
  	listen(socketfd, 5);
	int i;
  	while(1) {
		clilen = sizeof(client);
     	connection = accept(socketfd, (struct sockaddr *) &client, &clilen);
		
     	if(connection < 0) {
        		continue;
     	}
 
     	len = read(connection, buffer, MAX_SZ); 
		if(strcmp(buffer,"check")==0){
			if(flag == 0){
				flag = 1;
				send(connection,"using",sizeof("using"),0);
			}
			else{
				send(connection,"free",sizeof("free"),0);
			}
		}
		else if(strcmp(buffer,"release")==0){
			printf("fork %d placed back on table\n",port);
			flag = 0;
		}
     	close(connection);
  	}  
}

void CREATE_FORK(int spoon,int port_range) {
  	int process = fork();
	if (process < 0) {
    	perror("too busy for new processes");
    	abort();
  	} 
	else if (process == 0) {
    	chopstick(spoon,port_range+spoon);
    	exit(0);
  	}
	wait(NULL);
}
    
