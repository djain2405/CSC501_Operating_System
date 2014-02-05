
/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <lab1.h>
#include <stdio.h>
extern unsigned long ctr1000;
unsigned long currSP;   /* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:       Upon entry, currpid gives current process id.
 *      Proctab[currpid].pstate gives correct NEXT state for
 *      current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

int compgoodness(int ppid)
{
        int goodness=-1;

        if(proctab[ppid].pcounter<=0)
            goodness = 0;
        else 
            goodness = proctab[ppid].pcounter+proctab[ppid].pprio;
        
        return goodness;
}

int getnextpid(int qid) 
{
	// return next pid from corresponding "queue" used by RANDOMSCHED
	int nextpid = -1;
	if(qid == 1){
		nextpid = ht_dequeue(&q1);
	}
	else if(qid == 2){
		nextpid = ht_dequeue(&q2);
	}
	else {
		nextpid = ht_dequeue(&q3);
	}

	return nextpid;
}
	
int resched()
{
	register struct     pentry  *optr;  /* pointer to old process entry */
	register struct     pentry  *nptr;  /* pointer to new process entry */

        int next_ppid  = 0;
        int isnewepoch = 1;
	int chosen     = -1;


        if(scheduleclass == RANDOMSCHED) {       
		
		//kprintf("Entered RANDOMSCHED\n");
		srand(0);

	    do{
		chosen = rand() % 100;
		
		if(chosen >=0 && chosen < 50) {
			// next pid should have priority between 99-66
			next_ppid = ht_dequeue(1);
		}
		else if(chosen >=50 && chosen < 80) {
			// next pid should have priority between 65-33
			next_ppid = ht_dequeue(2);
		}
		else {
			// next pid should have priority between 32-0
			next_ppid = ht_dequeue(3);
		}
	
	    } while(next_ppid<0);
	        
		//kprintf("next_ppid=%d, currpid=%d\n",next_ppid,currpid);
		
		optr = &proctab[currpid];
		nptr = &proctab[next_ppid];
		
		ht_enqueue(optr->pprio, currpid);
		
		// check if next one is also current process
		if(next_ppid == currpid) {
			#ifdef RTCLOCK
			preempt = QUANTUM;
			#endif
			return OK;
		}

		// force context switch
		if (optr->pstate == PRCURR) {
	                optr->pstate = PRREADY;
                	insert(currpid,rdyhead,optr->pprio);
        	}

		nptr = &proctab[ (currpid = dequeue(next_ppid)) ];

		nptr->pstate = PRCURR;
		currpid = next_ppid;

		#ifdef RTCLOCK
		preempt = QUANTUM;
		#endif
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		
		// The OLD process returns here when resumed.
		return(OK);

        }
        else if(scheduleclass == LINUXSCHED) {

		//kprintf("Entered LINUXSCHED\n");
        	int i = 0;
                
                // setting the counter , quantum and epoch
                proctab[currpid].pcounter =  preempt;
                nepoch = nepoch - (proctab[currpid].pquantum-preempt);          
                
                //check if process is in new epoch
            	for(i = 1;i < NPROC; i++) {
                	if(proctab[i].pstate == PRCURR || proctab[i].pstate == PRREADY){
                        	if(compgoodness(i) > 0)
                                	isnewepoch = 0;
                    	}
            	}
        
                if(isnewepoch==1) {
                        nepoch=0;
	        	// set counter, quantum and epoch for the processes in the current epoch
                	for(i=1;i<NPROC;i++) {          
                	       	proctab[i].pprio = proctab[i].newprio;
                                proctab[i].pquantum = floor(proctab[i].pcounter/2) + proctab[i].pprio;
				proctab[i].pcounter = proctab[i].pquantum;
                                nepoch = nepoch + proctab[i].pquantum;              
                	}
                } 
                
                //pick the next highest priority process 
                int max = 0, goodness;
		for(i = 0;i < NPROC;i++) {
                	if(proctab[i].pstate == PRCURR || proctab[i].pstate == PRREADY) {
                            	
				goodness = compgoodness(i);
                            	if(goodness > max) {
                                    	max = goodness;
                                    	next_ppid = i;
                            	}
                    	}
           	}
                
                // inserting the current process in the queue
                if (optr->pstate == PRCURR) {
                	optr->pstate = PRREADY;
                	insert(currpid,rdyhead,optr->pprio);
        	}
        
                // current and next process are the same set the preempt to counter
                if(next_ppid==currpid) {
                        preempt = proctab[next_ppid].pcounter;
			optr->ticks += (QUANTUM - preempt);
                        return(OK);
                }
                else {
			optr->ticks += (QUANTUM - preempt);
                        //defining old and new process entry pointers
                        optr = &proctab[currpid];
                        nptr = &proctab[next_ppid];
                        if (optr->pstate == PRCURR)
                        optr->pstate = PRREADY;
                        currpid = next_ppid;
                        nptr->pstate = PRCURR;
                        
                        nptr = &proctab[ (currpid = dequeue(next_ppid)) ];

			#ifdef  RTCLOCK
                        preempt = nptr->pcounter;              // reset preemption counter
                        #endif

                        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                        return(OK);
                }

        }
        else{
		//kprintf("Entered DEFAULT\n");

		/* no switch needed if current process priority higher than next*/
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
		   (lastkey(rdytail)<optr->pprio)) {
			optr->ticks += (QUANTUM - preempt);
			return(OK);
		}

		optr->ticks += (QUANTUM - preempt);

		/* force context switch */
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
		#endif

		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

		/* The OLD process returns here when resumed. */
		return OK;        

	}
        
        return OK;
}
