/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

int prX;
void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
prch(c)
char c;
{
	int i;
	sleep(5);	
}
int main()
{
	kprintf("Houjun Tang (htang4)\n-----------------------\n");
	kprintf("Task 1 (zfunction)\n");
	long value = 0xaabbccdd;
	kprintf("Input: 0x%08x\nOutput: 0x%08x\n", value, zfunction(value));

	printsegaddress();
	
	printtos();

	printprocstks(0);

	syscallsummary_start();        
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	sleep(1);
	sleep(1);
	sleep(1);
	syscallsummary_stop();
	printsyscallsummary();

	return 0;
}
