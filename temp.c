#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <cv.h>
#include <spinlock.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

void handler();

void handler(){
	fprintf(stderr,"Wake Up, %d\n", getpid());
}	

void main(){
	/*sigset_t sigmask;
	signal(SIGINT,handlerInt);
	if(sigemptyset(&sigmask)<0){
		fprintf(stderr, "Sig Set Error: %s\n",strerror(errno));
		exit(1);
	}
	sigsuspend(&sigmask);
	char *fault;
	char *fault2;
	strcpy(fault,fault2);
	fprintf(stdout,"Signal Got Blocked\n");*/
	signal(SIGUSR1,handler);
	char *addr;
	char *addr2;
	if(((void*)(addr=mmap(NULL, (size_t)(sizeof(char)*4096),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0)))<0){
		fprintf(stderr,"MMAP ERROR: %s\n", strerror(errno));
		exit(1);
	}
	struct cv *test=(struct cv *) (addr);
	struct spinlock *mutex;
	if(((void*)(addr2=mmap(NULL,(size_t)(sizeof(char)*4096),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0)))<0){
		fprintf(stderr,"MMAP spinlock ERROR: %s\n", strerror(errno));
		exit(1);
	}
	mutex=(struct spinlock *) (addr2);
	cv_init(test);
	for(int i=3;i>0;i--){
		pid_t pid;
		switch(pid=fork()){
			case -1:
				fprintf(stderr,"Fork Error: %s\n", strerror(errno));
				exit(1);
			case 0:
				spin_lock(mutex);
				fprintf(stderr,"Enter Process: %d\n",getpid());
				cv_wait(test,mutex);
				spin_unlock(mutex);
				fprintf(stderr,"Exit Process: %d\n",getpid());
				exit(0);
			default:
				fprintf(stderr,"Fork Process %d\n",pid);
		}
	}
	printf("^^^^^^\n");
	getchar();
	perror("");
	cv_signal(test);
	cv_signal(test);
	cv_signal(test);
	int wstatus=0;
        pid_t pid;
        while((pid=wait(&wstatus))>0){
                fprintf(stderr, "EXIT %d\n", (int) pid);
                if(WIFSIGNALED(wstatus)){
                        fprintf(stderr,"Kill by Signal: %d\n", WTERMSIG(wstatus));
                }
        }
}
