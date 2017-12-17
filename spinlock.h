#ifndef SPINLOCK_H
#define SPINLOCK_H

struct spinlock{
	char primeLock;
};

void spin_lock(struct spinlock *l);
void spin_unlock(struct spinlock *l);
int tas(volatile char *lock);

#endif	//SPINLOCK_H
