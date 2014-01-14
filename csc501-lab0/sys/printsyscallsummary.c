#include <stdio.h>
//#include <string.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

int syscall_trace_on = 0;
int syscall_cnt[NPROC][27];
int syscall_time[NPROC][27];
int syscall_used[NPROC];
const char syscall_name[27][16]={"freemem", "chprio", "getpid", "getprio", "gettime", "kill", "receive", "recvclr", "recvtim", "resume", "scount", "sdelete", "send", "setdev", "setnok", "screate", "signal", "signaln", "sleep", "sleep10", "sleep100", "sleep1000", "sreset", "stacktrace", "suspend", "unsleep", "wait"};


void syscallsummary_start()
{
    int i, j;
    
    // turn trace on
    syscall_trace_on = 1;

    // init values
    for(i = 0; i < NPROC; ++i) {
	syscall_used[i] = 0;
	for(j = 0; j < 27; ++j) {
	    syscall_cnt[i][j] = 0;
    	    syscall_time[i][j] = 0;
	}
    }

}

void syscallsummary_stop()
{
    syscall_trace_on = 0;
}

void printsyscallsummary()
{
    int i, j;

    kprintf("\nTask 5 (printsyssummary)\n");
    
    // iterate all pid
    for(i = 0; i < NPROC; ++i) {

        // check only for valid pid
        if( syscall_used[i] == 0 ) {
            continue;
        }
        else {
	
	    kprintf("Process [pid:%d]\n", i);
            
            // iterate all syscall statistics gathered
            for(j = 0; j < 27; ++j) {
                
                if(syscall_cnt[i][j] > 0) {
		    
             	    kprintf("\tSyscall: %s, count: %d, average execution time: %d (ms)\n", syscall_name[j], syscall_cnt[i][j], syscall_time[i][j]/syscall_cnt[i][j]);

                }
                
            }

        }
    }

}

