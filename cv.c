#include <stdio.h>
#include <cv.h>
#include <spinlock.h>

void cv_init(struct cv *cv){
	memset((void *)cv,0,(size_t)sizeof(byte)*4096));
}

void cv_broadcast(struct cv *cv){

}
