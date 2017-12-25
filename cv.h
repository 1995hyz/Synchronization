#ifndef CV_H
#define CV_H

#define CV_MAXPROC 64

#include <signal.h>
#include <spinlock.h>

struct cv{
	struct spinlock cvmutex;
	int pidArray[CV_MAXPROC];
	int totalProcess;
	sigset_t sigmask;
}; 	

void cv_init(struct cv *cv);
void cv_wait(struct cv *cv, struct spinlock *mutex);
int cv_broadcast(struct cv *cv);
int cv_signal(struct cv *cv);

#endif	//CV_H
