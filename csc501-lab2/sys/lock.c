// lock.c
#include <kernel.h>
#include <stdio.h>
#include <q.h>
#include <proc.h>
#include <lock.h>

int lock(int ldes1, int type, int priority) {

    STATWORD ps;    
    struct  lentry  *lptr;
    struct  lentry  *tmplptr;
    struct  pentry  *pptr;
    
    int lock = (int)(ldes1/10);
    int locki= ldes1 - lock * 10;
    int wait = 0;
    int nr, nw, item, i;
    int tmpprio;

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
        pptr->plock  = (int)(ldes1 / 10);

        insert(currpid, lptr->lqhead, priority);

        q[currpid].qtype   = type;
        q[currpid].qtime   = ctr1000;
        pptr->plockret     = OK;

        // update lprio (The current inherited priority of the process)
        update_lprio(lock);

        // update pinh for all procs that hold this lock.
        tmplptr = &locks[lock];
        for(i=0; i<NPROC; i++) {
            if(tmplptr->pidheld[i] > 0)
                update_pinh(i);
        }

        resched();
        restore(ps);
        return pptr->plockret;
    }

    lptr->pidheld[currpid]          = 1;
    proctab[currpid].lockheld[lock] = 1;
    update_pinh(currpid);

    if (type == READ)
        lptr->lnr++;
    else
        lptr->lnw++;
    
    restore(ps);
    return(OK);
}

void update_lprio(int lid)
{
    int item, maxprio = -1;
    struct lentry *lptr = &locks[lid];

    int tmplockid = q[lptr->lqtail].qprev;
    while (tmplockid!= lptr->lqhead) {
        if (proctab[tmplockid].pprio > maxprio)
            maxprio = proctab[tmplockid].pprio;
        tmplockid = q[tmplockid].qprev;
    }
    lptr->lprio = maxprio;
}

void update_pinh(int pid)
{
    int i, maxprio = -1;
    struct pentry *pptr = &proctab[pid];

    for(i=0; i<NLOCKS; i++) {
        // find lock held by the process
        if(pptr->lockheld[i] > 0) {
            if (locks[i].lprio > maxprio)
                maxprio = locks[i].lprio;
        }
    }

    if(pptr->pprio > maxprio)
        pptr->pinh = 0;
    else
        pptr->pinh = maxprio;

}
