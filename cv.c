#include <stdio.h>
#include <cv.h>
#include <spinlock.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

void cv_init(struct cv *cv){
	cv->totalProcess=0;
	char *cvaddr=mmap(NULL,(size_t)(sizeof(char)*64),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	cv->cvmutex=(struct spinlock *)cvaddr;
	cv->cvmutex->primeLock=0;
}

void cv_wait(struct cv *cv, struct spinlock *mutex){
	sigset_t sigmask;
	if(sigfillset(&sigmask)<0){
		fprintf(stderr, "Sig Set Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(sigdelset(&sigmask,10)<0){
		fprintf(stderr, "Sig Delete Set Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	spin_lock(cv->cvmutex);
	cv->totalProcess++;
	pid_t currentpid=getpid();
	cv->pidArray[cv->totalProcess-1]=(int)currentpid;
	printf("%d\n",currentpid);
	spin_unlock(cv->cvmutex);
	spin_unlock(mutex);
	printf("$$$$\n");
	sigsuspend(&sigmask);
	spin_lock(mutex);
}
int cv_broadcast(struct cv *cv){
	spin_lock(cv->cvmutex);
	for(cv->totalProcess;cv->totalProcess>=0;cv->totalProcess--){
		kill((pid_t)cv->pidArray[cv->totalProcess-1],10);
	}
	spin_unlock(cv->cvmutex);
}
int cv_signal(struct cv *cv){
	spin_lock(cv->cvmutex);
	cv->totalProcess--;
	kill((pid_t)cv->pidArray[cv->totalProcess],10);
	spin_unlock(cv->cvmutex);
}
