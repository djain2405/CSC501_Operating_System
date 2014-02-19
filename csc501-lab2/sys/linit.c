// void linit(void)
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

struct lentry locks[NLOCKS];
int nextlock;
int lockiter;

void linit(void) {

    struct lentry *lptr;
    int i;

    nextlock = NLOCKS - 1;
    lockiter = 0;

    for (i = 0; i < NLOCKS; i++) {
        (lptr = &locks[i])->lstate = LFREE;
        lptr->lqtail = 1 + (lptr->lqhead = newqueue()); 
	lptr->liter = 0;	
    }

}
