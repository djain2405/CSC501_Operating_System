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
            restore(ps);
            return(SYSERR);
        }

        // verify the lock request is in current locks iteration
        if (locki != lockiter) {
            restore(ps);
            return(SYSERR);
        }

        // update the count for this resource
        if (lptr->lnw > 0)
            lptr->lnw--;
        else if(lptr->lnr > 0)
            lptr->lnr--;

        // Update the lock's bitvector so that it can know what procs hold it
        //clr_bit(lptr->lprocs_bsptr, currpid);
        // Update the proc's bitvector so that it can know what locks it holds
        //clr_bit(proctab[currpid].locks_bsptr, lock);
        // Update the priority of the process that is releasing the lock
        //update_priority(currpid);


        // no waiting procs then continue
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

    register struct lentry *lptr = &locks[lock];

    // update the reader/writer counters
    if (q[item].qtype == READ)
        lptr->lnr++;
    else if (q[item].qtype == WRITE)
        lptr->lnw++;

    // Update the lock's bitvector so that it can know what procs hold it
    //set_bit(lptr->lprocs_bsptr, item);
    // Update the proc's bitvector so that it can know what locks it holds
    //set_bit(proctab[item].locks_bsptr, lock);
    // Update lppriomax for this lock (max priority of all waiting procs)
    //update_lppriomax(lock);
    // Update pinh for all procs that hold this lock.
    //update_pinh(lock);

    dequeue(item);
    ready(item, RESCHNO);
}
