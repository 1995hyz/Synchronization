#include <spinlock.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/wait.h>

//spintest [n1] [n2]		n1 is amount of spawns, n2 is amount of iterations

void main(int argc, char *argv[]){
	int spawnsNum=0;
	int iterationNum=0;
	int fd;
	char *addr;
	int buf[2]={0,'\0'};
	if(argc!=3){
		fprintf(stderr,"Invalid Arguments\n");
		exit(1);
	}
	spawnsNum=atoi(argv[1]);
	iterationNum=atoi(argv[2]);
	if((spawnsNum<1)||(iterationNum<1)){
		fprintf(stderr,"Invalid Arguments\n");
		exit(EXIT_FAILURE);
	}
	if((fd=open("./$spinlocktest.txt",O_RDWR|O_CREAT|O_TRUNC,0666))<0){
		fprintf(stderr,"Fail to Creat Test File, Error: %s\n",strerror(errno));
		exit(1);
	}
	write(fd,buf,sizeof(int)*2);
	size_t length=sizeof(int)*2+2000;
	if(((void *)(addr=mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,fd,0))<0)){
		fprintf(stderr,"Map File Error: %s\n",strerror(errno));
		remove("./$spinlocktest.txt");
		exit(EXIT_FAILURE);
	}
	pid_t pid=0;
	int temp=1;
	char buf3[2]={'0','\0'};
	struct spinlock *l;
	l=(struct spinlock *)(addr+sizeof(int)*8);
	l->primeLock=0;		
	while(spawnsNum){
		if((pid=fork())<0){
			fprintf(stderr,"Fork Error: %s\n",strerror(errno));
			remove("./$spinlocktest.txt");
			exit(EXIT_FAILURE);	
		}
		switch(pid){
			case 0:
				while(iterationNum){	
							spin_lock(l);
							memcpy(&temp,addr,sizeof(int));
							temp++;
							for(int i=0;i<2;i++){		//delay adding process
								;
							}
							memcpy(addr,&temp,sizeof(int));
							spin_unlock(l);			
							iterationNum--;
				}
				exit(EXIT_SUCCESS);
			default:
				fprintf(stderr,"Fork Child Process %d\n",(int) pid);
		}
		spawnsNum--;
	}
	int wstatus=0;
	while((pid=wait(&wstatus))>0){
		fprintf(stderr, "EXIT %d\n",(int) pid);
		if(WIFSIGNALED(wstatus)){
			fprintf(stderr,"Kill by Signal: %d\n",WTERMSIG(wstatus));
		}
	}
	int buf2[2]={0,'\0'};
	memcpy(buf2,addr,sizeof(int));
	fprintf(stderr,"Final Result: %d\n",buf2[0]);
	remove("./$spinlocktest.txt");
}
		
