#include "./FORK.c"

int main(int argc,char *argv[]){
	if(argc < 2){
		perror("incorrect input parameters\n");
		return 0;
	}
	CREATE_FORK(3,atoi(argv[1]));
	return 1;
}
    
