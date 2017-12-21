#ifndef CV_H
#define CV_

#define CV_MAXPROC 64
struct cv{
	struct spinlock *cvmutex;
	int pidArray[CV_MAXPROC];
	int totalProcess;
}; 	

void cv_init(struct cv *cv);
void cv_wait(struct cv *cv, struct spinlock *mutex);
int cv_broadcast(struct cv *cv);
int cv_signal(struct cv *cv);

#endif	//CV_H
