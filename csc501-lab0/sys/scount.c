/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
        // added for PA0 tracing
        int start_time;
        int curridx = 10;
        if(syscall_trace_on == 1) {
                syscall_used[currpid] = 1;
                syscall_cnt[currpid][curridx]++;
                start_time = ctr1000;
        }

	extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE)
		return(SYSERR);

        // added for PA0 tracing
        if(syscall_trace_on == 1) {
                syscall_time[currpid][curridx] += ctr1000 - start_time;
        }


	return(semaph[sem].semcnt);
}
