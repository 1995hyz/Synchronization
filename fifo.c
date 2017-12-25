#include <fifo.h>
#include <cv.h>
#include <spinlock.h>
#include <stdio.h>
#include <fifo.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void fifo_init(struct fifo *f){
	f->fifo_mutex.primeLock=0;
	cv_init(&f->full);
	cv_init(&f->empty);
	f->write_index=0;
	f->read_index=0;	
	f->long_count=0;
	int i=0;
	for(i;i<MYFIFO_BUFSIZ;i++){
		f->buf[i]=0;
	}
	fprintf(stderr, "Fifo init successfully\n");
}

void fifo_wr(struct fifo *f, unsigned long d){
	spin_lock(&f->fifo_mutex);
	while(f->long_count>=MYFIFO_BUFSIZ){
		cv_wait(&f->full,&f->fifo_mutex);
	}
	int tempindex=f->write_index;
	f->buf[f->write_index++]=d;
	//fprintf(stderr,"Write Value %lu%lu to %d\n",f->buf[tempindex]>>16,f->buf[tempindex]&65535,tempindex);
	f->long_count++;
	f->write_index%=MYFIFO_BUFSIZ;
	cv_signal(&f->empty);
	spin_unlock(&f->fifo_mutex);
}

unsigned long fifo_rd(struct fifo *f){
	unsigned long d;
	spin_lock(&f->fifo_mutex);
	while(f->long_count<=0){
		cv_wait(&f->empty,&f->fifo_mutex);
	}
	int tempread=f->read_index;
	d=f->buf[f->read_index++];
	f->read_index%=MYFIFO_BUFSIZ;
	f->long_count--;
	cv_signal(&f->full);
	spin_unlock(&f->fifo_mutex);
	//fprintf(stderr,"***** Read Value: %lu%lu from %d\n",d>>16,d&65535,tempread);
	return d;	
}
