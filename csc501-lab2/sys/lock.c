// lock.c
#include <kernel.h>
#include <stdio.h>
#include <q.h>
#include <proc.h>
#include <lock.h>

int lock(int ldes1, int type, int priority) {

    STATWORD ps;    
    struct  lentry  *lptr;
    struct  pentry  *pptr;
    
    int lock = (int)(ldes1/10);
    int locki= ldes1 - lock * 10;
    int wait = 0;
    int nr, nw, item;

    disable(ps);
    
    lptr = &locks[lock];
    if (isbadlock(lock) || lptr->lstate == LFREE) {
        restore(ps);
        return(SYSERR);
    }

    // Verify the lock request is in current locks iteration
    if (locki != lockiter) {
        restore(ps);
        return(SYSERR);
    }

    nr = lptr->lnr;
    nw = lptr->lnw;
    if (nr == 0 && nw == 0) {
        // no lock at all
        wait = 0;
    }
    else if ( (nr == 0 && nw != 0) || (nr != 0 && nw == 0 && type == WRITE) ) {
        // write lock or read lock and requested by write
        wait = 1;   
    }
    else if (nr != 0 && nw == 0 && type == READ) {
        
	item = q[lptr->lqtail].qprev;
        while (priority < q[item].qkey) { 
	    // check if need to wait for higher priority WRITE
            if (q[item].qtype == WRITE)
                wait = 1;
            item = q[item].qprev;
        }
    }


    if (wait) {
        pptr = &proctab[currpid];
        pptr->pstate = PRLOCK;
        pptr->plock  = ldes1;

        insert(currpid, lptr->lqhead, priority);

        q[currpid].qtype   = type;
        q[currpid].qtime   = ctr1000;
        pptr->plockret     = OK;

        // Update lppriomax for this lock (max priority of all waiting procs)
        //update_lppriomax(lock);
        // Update pinh for all procs that hold this lock.
        //update_pinh(lock);
        
        resched();
        restore(ps);
        return pptr->plockret;
    }


    // Update the lock's bitvector so that it can know what procs hold it
    //set_bit(lptr->lprocs_bsptr, currpid);
    // Update the proc's bitvector so that it can know what locks it holds
    //set_bit(proctab[currpid].locks_bsptr, lock);
    // Update pinh for this process 
    //update_priority(currpid);


    // Increment the reader/writer count and move on.
    if (type == READ)
        lptr->lnr++;
    else
        lptr->lnw++;
    
    restore(ps);
    return(OK);
}

