/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
SYSCALL	receive()
{
        // added for PA0 tracing
        int start_time;
        int curridx = 6;
        if(syscall_trace_on == 1) {
                syscall_used[currpid] = 1;
                syscall_cnt[currpid][curridx]++;
                start_time = ctr1000;
        }

	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;

	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);

        // added for PA0 tracing
        if(syscall_trace_on == 1) {
                syscall_time[currpid][curridx] += ctr1000 - start_time;
        }
	return(msg);
}
