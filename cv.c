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
	int i=CV_MAXPROC;
	for(i;i>=0;i--){
		cv->pidArray[i]=0;
	}
	cv->cvmutex.primeLock=0;
}

void cv_wait(struct cv *cv, struct spinlock *mutex){
	spin_lock(&cv->cvmutex);
	cv->totalProcess++;
	pid_t currentpid=getpid();
	cv->pidArray[cv->totalProcess-1]=(int)currentpid;
	spin_unlock(&cv->cvmutex);
	spin_unlock(mutex);
	sigsuspend(&cv->sigmask);
	spin_lock(mutex);
}
int cv_broadcast(struct cv *cv){
	spin_lock(&cv->cvmutex);
	for(cv->totalProcess;cv->totalProcess>=0;cv->totalProcess--){
		kill((pid_t)cv->pidArray[cv->totalProcess-1],SIGUSR1);
	}
	spin_unlock(&cv->cvmutex);
}
int cv_signal(struct cv *cv){
	spin_lock(&cv->cvmutex);
	if(cv->totalProcess>0){
		cv->totalProcess--;
		kill((pid_t)cv->pidArray[cv->totalProcess],SIGUSR1);
	}
	spin_unlock(&cv->cvmutex);
}
