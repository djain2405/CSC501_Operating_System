// int lcreate(void)
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id
 *------------------------------------------------------------------------
 */
int lcreate(void)
{
	STATWORD ps;    
	int	 lock;
	
	disable(ps);
	if ( (lock=newlock())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	
	restore(ps);
	return(lock);
}

/*------------------------------------------------------------------------
 * newlock  --  allocate an unused lock and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock()
{
	int lock;
	int i;

	for (i=0 ; i<NLOCKS; i++) {
		lock=nextlock--;
	
		if (nextlock < 0) {
			nextlock = NLOCKS - 1;
			lockiter++;
		}
		
		if (locks[lock].lstate==LFREE) {
			locks[lock].lstate = LUSED;
			locks[lock].lnr    = 0;
		        locks[lock].lnw    = 0;

			return(lock*10+lockiter);
		}
	}
	return(SYSERR);
}
