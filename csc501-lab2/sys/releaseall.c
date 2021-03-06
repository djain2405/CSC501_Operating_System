// releaseall.c
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include <lock.h>

LOCAL void admitreader(int ldes);
LOCAL void unblock(int ldes, int item);

int releaseall(numlocks, ldes1)
    int numlocks;
    int ldes1;
{
    STATWORD ps;    
    register struct lentry *lptr;
    int i, item;
    int priority;
    int lock, locki;
    int longread, longwrite;
    int choice;
    unsigned long maxreadtime, maxwritetime;
    int anyerr = 0;

    disable(ps);
    
    for (i = 0; i < numlocks; i++) {

        lock = (int)(*((&ldes1) + i) / 10);
        locki= *((&ldes1) + i) - lock * 10;
        lptr = &locks[lock];
        priority     = 0;
	longread     = -1;
	longwrite    = -1;
	maxreadtime  = 0;
	maxwritetime = 0;
	choice       = 0;

        // verify lock
        if (isbadlock(lock) || lptr->lstate==LFREE) {
            anyerr = 1;
            continue;
        }

        // verify the lock request is in current locks iteration
        if (locki != lockiter) {
            anyerr = 1;
            continue;
        }

        // update the count for this resource
        if (lptr->lnw > 0)
            lptr->lnw--;
        else if(lptr->lnr > 0)
            lptr->lnr--;

        lptr->pidheld[currpid]          = 0;
        proctab[currpid].lockheld[lock] = 0;
        update_pinh(currpid);

        if (isempty(lptr->lqhead))
            continue;

        // choose proc with highest priority
        item = q[lptr->lqtail].qprev;

        // check if current item in queue has same priority with its prev 
        if (q[item].qkey != q[q[item].qprev].qkey) {
	    // no equal priority
            if (q[item].qtype == READ && lptr->lnw == 0) {
                // chose READ to be the next
                admitreader(lock);
            }
            else if (q[item].qtype == WRITE && lptr->lnw == 0 && lptr->lnr ==0) {
                // chose WRITE to be next
                unblock(lock, item);
            }
            continue;
        }
	else{
	    // equal waiting priority, process with longest waiting time aquire the lock. 
            priority = q[item].qkey;
            
	    while (q[item].qkey == priority) { 

		// find longest READ and WRITE if any		
		if (q[item].qtype == READ && q[item].qtime > maxreadtime)
		    longread = item;
		else if (q[item].qtype == WRITE && q[item].qtime > maxwritetime)
		    longwrite = item;

		if (longread >= 0 && longwrite >=0) {
		    if ( abs(maxreadtime-maxwritetime) < 1000){
			choice = longwrite;	
		    }
		    else if(maxreadtime > maxwritetime) {
			choice = longread;
		    }
		    else if(maxreadtime < maxwritetime) {
			choice = longwrite;
		    }
		    
		}
		else if (longread >= 0 && longwrite == 0 ) {
		    choice = longread;
		}
		else if (longread == 0 && longwrite >= 0 ) {
                    choice = longwrite;
                }
		else {
		    // shouldn't be happening
		    choice = -1;
		}

                item = q[item].qprev;
            }


    	    if (q[choice].qtype == READ && lptr->lnw == 0) {
		// chose READ to be the next
		admitreader(lock);
	    }
	    else if (q[choice].qtype == WRITE && lptr->lnw == 0 && lptr->lnr ==0) {
		// chose WRITE to be next
		unblock(lock,choice);
	    }
	}
    }

    if(anyerr) {
        restore(ps);
        return SYSERR;
    }

    restore(ps);
    resched();
    return(OK);
}

LOCAL void admitreader(int lock) {
    // if a reader is chosen to have a lock, then all the other waiting readers having priority not less than that of the highest-priority waiting writer for the same lock should also be admitted. 

    register struct lentry *lptr = &locks[lock];
    int item, tmpitem, threshold = -100000;

    // find the highest-priority waiting writer
    item = q[lptr->lqtail].qprev;
    while (item != lptr->lqhead) { 
        if (q[item].qtype == WRITE && q[item].qkey > threshold)
            threshold = q[item].qkey;
        item = q[item].qprev;
    }

    item = q[lptr->lqtail].qprev;
    while (item != lptr->lqhead) { 
        if (q[item].qtype == READ && q[item].qkey >= threshold) {
	    tmpitem = q[item].qprev;
            unblock(lock, item);
            item = tmpitem;
        }
    }
}

LOCAL void unblock(int lock, int item) {

    int i;
    register struct lentry *lptr = &locks[lock];

    // update the reader/writer counters
    if (q[item].qtype == READ)
        lptr->lnr++;
    else if (q[item].qtype == WRITE)
        lptr->lnw++;

    lptr->pidheld[item]             = 1;
    proctab[currpid].lockheld[lock] = 1;
    update_lprio(lock);
    for(i=0; i<NPROC; i++) {
        if(lptr->pidheld[i] > 0)
            update_pinh(i);
    }

    dequeue(item);
    ready(item, RESCHNO);
}
