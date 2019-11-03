#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define PHILOSOPHERS 5
#define MAX_SZ 1024
 
void START_MONITOR(int port_no){
    char buffer[MAX_SZ];
	
    FILE *file_output;
	file_output = fopen("output.txt","w");
	fputs("Current time \t\t\t philo #0 \t philo #1 \t philo #2 \t philo #3 \t philo #4 \n",file_output);
	fclose(file_output);

	int socketfd;
    if ( (socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("creating socket failed");
        exit(EXIT_FAILURE);
    }
	struct sockaddr_in server, client;
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

	int PORT = port_no + 5;
    server.sin_family    = AF_INET; 
    server.sin_addr.s_addr = INADDR_ANY;		
    server.sin_port = htons(PORT);
    if (bind(socketfd, (const struct sockaddr *)&server, sizeof(server)) < 0 )
    {
        perror("binding failed due to busy port");
        exit(EXIT_FAILURE); 
    }
     
   int len, n;
	printf("Current time \t\t\t philo #0 \t philo #1 \t philo #2 \t philo #3 \t philo #4\n");
	time_t timep;
    struct tm *time_info;
	char time_string[MAX_SZ];
	while(1)
	{
		file_output=fopen("output.txt","a+");
	    n = recvfrom(socketfd, (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&client,&len);
		time(&timep);
		time_info = localtime(&timep);    
		buffer[n] = '\n';
		buffer[n+1] = '\0';
		int count=0;
		sprintf(time_string, "%d/%02d/%02d %02d:%02d:%02d", (1900+time_info->tm_year), (1+time_info->tm_mon), time_info->tm_mday, (time_info->tm_hour), time_info->tm_min, time_info->tm_sec);
		printf("%s:	%s\n",time_string,buffer);
		fputs(time_string,file_output);
		fputs(buffer,file_output);
		fputc('\n',file_output);	
		strcpy(buffer,"");
		fclose(file_output);
	}
}

int main(int argc,char *argv[]){
	if(argc < 2){
		perror("incorrect input parameters\n");
		return 0;
	}
	START_MONITOR(atoi(argv[1]));
}


