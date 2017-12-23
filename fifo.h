#ifndef FIFO_H
#define FIFO_H

#define MYFIFO_BUFSIZ 1024

struct fifo{
	long buf[MYFIFO_BUFSIZ];
	int write_index;
	int read_index;
	int long_count;
	struct spinlock *fifo_mutex;
	struct cv* full;
	struct cv* empty;
};

void fifo_init(struct fifo *f);
void fifo_wr(struct fifo *f,unsigned long d);
unsigned long fifo_rd(struct fifo *f);

#endif
