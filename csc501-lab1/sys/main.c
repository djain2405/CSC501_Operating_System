#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <stdio.h>
#include <math.h>
#include <lab1.h>

#define LOOP	50

int prA, prB, prC;
int proc_a(char c);
int proc_b(char c);
int proc_c(char c);
int proc(char c);
int proc_basic(char c);
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;

int main() {
	int i;
	int count = 0;

	ps();
/*
	// the second part
	setschedclass(RANDOMSCHED);
	//setschedclass(LINUXSCHED);
	//setschedclass(DEFAULT);
	prA = create(proc_a, 2000, 30, "proc A", 1, 'A');
	prB = create(proc_b, 2000, 60, "proc B", 1, 'B');
	prC = create(proc_c, 2000, 90, "proc C", 1, 'C');
	kprintf("A:%d B:%d C:%d Created\n",prA,prB,prC);

	resume(prA);
	resume(prB);
	resume(prC);

	kprintf("Before sleep()\n");
	sleep(2);

	kprintf("After sleep()\n");

	kill(prA);
	kill(prB);
	kill(prC);
*/	
	// the third part
	setschedclass(LINUXSCHED);
	resume(prA = create(proc, 2000, 5, "proc A", 1, 'A'));
	resume(prB = create(proc, 2000, 50, "proc B", 1, 'B'));
	resume(prC = create(proc, 2000, 90, "proc C", 1, 'C'));
	while (count++ < LOOP) {
		if(count == LOOP/2)
			ps();
		kprintf("M");

		for (i = 0; i < 10000000; i++);
	}
	return 0;
}

proc_basic(char c) {
	int count = 0;

	//kprintf("\nStart %c...\n", c);
	while (count++ < 1000) {
		kprintf("%c", c);
	}
}

proc_a(char c)
{
	int i;
	kprintf("Start... %c\n", c);
	b_cnt = 0;
	c_cnt = 0;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		a_cnt++;
	}
}

proc_b(char c)
{
	int i;
	a_cnt = 0;
	c_cnt = 0;

	kprintf("Start... %c\n", c);
	while (1) {
		for (i = 0; i < 10000; i++)
			;
		b_cnt++;
	}
}

proc_c(char c)
{
	int i;
	a_cnt = 0;
	b_cnt = 0;

	kprintf("Start... %c\n", c);
	while (1) {
		for (i = 0; i < 10000; i++)
			;
		c_cnt++;
	}
}


proc(char c) {
	int i;
	int count = 0;

	while (count++ < LOOP) {
		kprintf("%c", c);
		for (i = 0; i < 10000000; i++);
	}

}
