#include <fifo.h>
#include <cv.h>
#include <spinlock.h>
#include <stdio.h>

void fifo_init(struct fifo *f){
	f->fifo_mutex->primeLock=0;
	cv_init(f->full);
	cv_init(f->empty);
	f->write_index=0;
	f->read_index=0;	
	f->long_count=0;
}

void fifo_wr(struct fifo *f, unsigned long d){
	spin_lock(f->fifo_mutex);
	while(f->long_count>MYFIFO_BUFSIZ){
		cv_wait(f->full,f->fifo_mutex);
	}
	f->buf[f->write_index++]=d;
	f->long_count++;
	f->write_index%=MYFIFO_BUFSIZ;
	cv_signal(f->empty);
	spin_unlock(f->fifo_mutex);
}

unsigned long fifo_rd(struct fifo *f){
	long d;
	spin_lock(f->fifo_mutex);
	while(f->long_count<=0){
		cv_wait(f->empty,f->fifo_mutex);
	}
	d=f->buf[f->read_index++];
	f->read_index%=MYFIFO_BUFSIZ;
	f->long_count--;
	cv_signal(f->full);
	spin_unlock(f->fifo_mutex);
	return d;	
}
