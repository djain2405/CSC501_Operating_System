#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <lab1.h>
char statename[11][10]={"", "PRCURR", "PRFREE", "PRREADY", "PRRECV", "PRSLEEP", "PRSUSP", "PRWAIT", "PRTRECV"};
typedef struct {
	int ticks;
	int pid;
}Tickpid;

void ps()
{

    int i, j, cnt;
    struct pentry *process;
    Tickpid tickpid[NPROC];
    Tickpid temp;

    kprintf("\nName\tPID\tPriority\tStatus\tTicks\tStackSize\n");

    cnt = 0;
    for(i = 0; i < NPROC; ++i) {
        process = &proctab[i];
        // validate pid
        if( process->pstate != PRFREE  ) {
		tickpid[cnt].ticks = process->ticks;
		tickpid[cnt].pid   = i;
		++cnt;		
        }
    }

    // sort accroding to ticks
    for(i = 1; i < cnt; ++i)
        for(j = 0; j < cnt - i; ++j){
            if(tickpid[j].ticks < tickpid[j+1].ticks) {
                 temp.ticks = tickpid[j].ticks;
                 temp.pid   = tickpid[j].pid;
                 tickpid[j].ticks = tickpid[j+1].ticks;
                 tickpid[j].pid	  = tickpid[j+1].pid;
                 tickpid[j+1].ticks = temp.ticks;
                 tickpid[j+1].pid   = temp.pid;
           }
      }

    for(i = 0; i < cnt; ++i) {
	process = &proctab[tickpid[i].pid];
        kprintf("%s\t%d\t%d\t\t%s\t%d\t%d\n", process->pname, tickpid[i].pid, process->pprio, statename[process->pstate], process->ticks, process->pstklen);
    }
}
