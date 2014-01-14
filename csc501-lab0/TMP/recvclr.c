/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
SYSCALL	recvclr()
{

        // added for PA0 tracing
        int start_time;
        int curridx = 7;
        if(syscall_trace_on == 1) {
                syscall_used[currpid] = 1;
                syscall_cnt[currpid][curridx]++;
                start_time = ctr1000;
        }

	STATWORD ps;    
	WORD	msg;

	disable(ps);
	if (proctab[currpid].phasmsg) {
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);

        // added for PA0 tracing
        if(syscall_trace_on == 1) {
                syscall_time[currpid][curridx] += ctr1000 - start_time;
        }
	return(msg);
}
