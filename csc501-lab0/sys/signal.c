/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL signal(int sem)
{

        // added for PA0 tracing
        int start_time;
        int curridx = 16;
        if(syscall_trace_on == 1) {
                syscall_used[currpid] = 1;
                syscall_cnt[currpid][curridx]++;
                start_time = ctr1000;
        }


	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);

        // added for PA0 tracing
        if(syscall_trace_on == 1) {
                syscall_time[currpid][curridx] += ctr1000 - start_time;
        }

	return(OK);
}
