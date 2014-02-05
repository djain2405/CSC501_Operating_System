/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int ready(int pid, int resch)
{
	//kprintf("%d:Entered ready()\n",pid);
	register struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;
	insert(pid,rdyhead,pptr->pprio);
	

	// insert pid to corresponding queue
	int tmpprio = pptr->pprio;
	if(tmpprio >=66 && tmpprio <=99) {
		ht_dequeue(1);
	}
	else if(tmpprio >=33 && tmpprio <=65) {
		ht_dequeue(2);
	}
	else {
		ht_dequeue(3);
	}

	if (resch)
		resched();
	
	//kprintf("%d:Exited ready()\n",pid);
	return(OK);
}
