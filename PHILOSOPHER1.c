#include "PHILOSOPHER.c"

int main (int argc, char **argv) {
	if(argc != 3){
		// ./philosopher#i ipaddress port
		perror("incorrect input in the parameters");
		return 0;
	}

	int philosopher = 1;

	printf("\t\t\t\t\tphilosopher_%d\n\n",philosopher);

	char *address;
	address = argv[1];
	int port_no = atoi(argv[2]);
	pid_t pid;
	int i;
	int n = 5;

  	if ((pid = fork()) < 0) {
    			perror("creating the child process failed");
    			abort();
  	} 
	else if (pid == 0) {
    	philosopherProcess(port_no, 0, address, philosopher);
    	exit(0);
  	}
	wait(NULL);
	sleep(1);
	int done=0;
	do{
		done = philosopherProcess(port_no, 1, address, philosopher);
	}while(done != 1);

}

