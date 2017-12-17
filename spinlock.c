#include <stdio.h>
#include <spinlock.h>
#include <string.h>

void spin_lock(struct spinlock *l){
	int temp=0;
	//temp=tas(&(l->primeLock));
	while((temp=tas(&(l->primeLock)))!=0){
		;
	}
}

void spin_unlock(struct spinlock *l){
	//char buf[2]={'0','\0'};
	//memcpy(l->primeLock,buf,sizeof(char));
	l->primeLock=0;
}
