#include <stdio.h>
#include <kernel.h>
#include <proc.h>

static unsigned long *esp;

void printprocstks(int priority)
{

    int i;
    struct pentry *process;

    kprintf("\nTask 4 (printprocstks)\n# Filter: %d\n", priority);

    for(i = 0; i < NPROC; ++i) {
        
        process = &proctab[i];

        // validate pid
        if( (process->pstate) == PRFREE || (process->pprio <= priority) ) {
            continue;
        }
        else {
            kprintf("Process [%s]\n", process->pname);
            kprintf("\tpid: %d\n", i); 
            kprintf("\tpriority: %d\n",process->pprio);
            kprintf("\tbase: 0x%08x\n", process->pbase);
            kprintf("\tlimit: 0x%08x\n", process->plimit);
            kprintf("\tlen: %d\n", process->pstklen);
            if( i == currpid) {
                // special case for current process
                asm("movl %esp,esp");
                kprintf("\t pointer: 0x%08x\n", esp);
            }
            else
                kprintf("\t pointer: 0x%08x\n", process->pesp);

        }

    }








}
