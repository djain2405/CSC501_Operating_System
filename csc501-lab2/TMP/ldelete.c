//  int ldelete (int lockdescriptor)
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
int ldelete (int lockdescriptor)
{
	STATWORD ps;    
	struct	 lentry	*lptr;
	
	int	 pid;
    	int 	 lock = (int)(lockdescriptor/10);
	int	 locki= lockdescriptor - lock * 10;

	disable(ps);

	if (isbadlock(lock) || locks[lock].lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	
	lptr = &locks[lock];
	lptr->lstate = LFREE;
	
	if (locki != lockiter) {
        	restore(ps);
	        return(SYSERR);
    	}
    
	if (nonempty(lptr->lqhead)) {
		while( (pid=getfirst(lptr->lqhead)) != EMPTY) {
		    proctab[pid].plockret = DELETED;
		    ready(pid,RESCHNO);
		}
		resched();
	}
	restore(ps);
	return(OK);
}
