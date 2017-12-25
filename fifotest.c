#include <stdio.h>
#include <spinlock.h>
#include <cv.h>
#include <fifo.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(){
	//fprintf(stderr,"Wake Up, %d\n", getpid());
	;
}
void main(int argc, char *argv[]){
        sigset_t sigmaskC;
        if(sigfillset(&sigmaskC)<0){
                fprintf(stderr, "Sig Set Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
        if(sigdelset(&sigmaskC,10)<0){
                fprintf(stderr, "Sig Delete Set Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }
	signal(SIGUSR1,handler);
	if(argc!=3){
		fprintf(stderr, "Invalid Argument\n");
		exit(EXIT_FAILURE);
	}
	int totalWriters=atoi(argv[1]);
	int totalItems=atoi(argv[2]);
	char *fifoAddr;
	if(((void *)(fifoAddr=mmap(NULL,sizeof(struct fifo),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0)))<0){
		fprintf(stderr,"MMAP struct fifo Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct fifo *f=(struct fifo *)fifoAddr;
	fifo_init(f);
	f->full.sigmask=sigmaskC;
	f->empty.sigmask=sigmaskC;
	pid_t reader=0;
	long unsigned readChar;
	reader=fork();
	int j=totalWriters;
	int temp=0;
	unsigned long templong=0;
	switch(reader){
		case -1:
			fprintf(stderr,"Fork Reader Error: %s\n",strerror(errno));
			exit(EXIT_FAILURE);
		case 0:
			do{	
				readChar=fifo_rd(f);
				//fprintf(stderr,"*****,return value: %lu%lu\n",readChar>>16,readChar&65535);
				if((readChar&65535)==(totalItems-1)){
					j--;
					temp++;
					printf("temp: %d\n",temp);
					fprintf(stderr,"Reader Stream %lu Closed, last read: %lu\n", (long unsigned int) readChar>>16,readChar&65535);
				}
			}
			while(j!=0);
			fprintf(stderr,"Reader Exit\n");
			exit(0);
		default:
			;
	}
	int forkCount=0;
	long unsigned iterationCount;
	long unsigned inputNumber;
	while(forkCount<totalWriters){
		pid_t writer=0;
		writer=fork();		
		switch(writer){
			case -1:
				fprintf(stderr,"Fork Writer %d Error: %s\n",forkCount, strerror(errno)); 
				exit(EXIT_FAILURE);
			case 0:
				iterationCount=0;	
				inputNumber=forkCount<<16;
				while(iterationCount<totalItems){
					iterationCount++;
					fifo_wr(f,inputNumber);
					inputNumber++;
				}
				fprintf(stderr,"Writer %d EXIT\n",forkCount);
				exit(0);
			default:
				//printf("Forked %d\n",writer);
				forkCount++;
		}
	}
	int wstatus=0;
	pid_t waitpid;
	while((waitpid=wait(&wstatus))>0){
		if(WIFSIGNALED(wstatus)){
		fprintf(stderr,"Process: %d Kill by Signal: %d\n",waitpid, WTERMSIG(wstatus));
		}
	}
}
