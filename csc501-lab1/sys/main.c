#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <stdio.h>
#include <math.h>
#include <lab1.h>

#define LOOP 50

int prA, prB, prC;
int proc_a(), proc_b(), proc_c();
int proc(char c);
int proc_basic(char c);
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;

int count=0;


int main() {
	int i;
	int count = 0;
	char buf[8];

	kprintf("\n\n########## Test Case1, ps():\n");
	ps();
	kprintf("################### Sleep 5 seconds...\n");
	sleep(5);
	ps();
	kprintf("\n\n########## Test Case2, linux-like scheduling:\n");
	setschedclass(LINUXSCHED);
	resume(prA = create(proc, 2000, 5, "proc A", 1, 'A'));
	resume(prB = create(proc, 2000, 50, "proc B", 1, 'B'));
	resume(prC = create(proc, 2000, 90, "proc C", 1, 'C'));
	while (count++ < LOOP) {
		kprintf("M");

		for (i = 0; i < 10000000; i++);
	}
	kill(prA);
	kill(prB);
	kill(prC);

	setschedclass(RANDOMSCHED);
	kprintf("SCHEDCLASS=%d\n",scheduleclass);
	prA = create(proc_a, 2000, 30, "proc A", 1, 'A');
	prB = create(proc_b, 2000, 60, "proc B", 1, 'B');
	prC = create(proc_c, 2000, 90, "proc C", 1, 'C');
	resume(prA);
	resume(prB);
	resume(prC);
	kprintf("\n\n########## Test Case3, ps():\n");
	kprintf("################### Sleep 5 seconds...\n");
	ps();
	sleep(5);
	kill(prA);
	kill(prB);
	kill(prC);
	double total_cnt;
	double a_percent, b_percent, c_percent;
	total_cnt = a_cnt + b_cnt + c_cnt;
	a_percent = (double) a_cnt / total_cnt * 100;
	b_percent = (double) b_cnt / total_cnt * 100;
	c_percent = (double) c_cnt / total_cnt * 100;

	kprintf("\n\n########## Test Case4, random-priority scheduling:\n");
	kprintf("Test RESULT: A = %d, B = %d, C = %d, (%d : %d : %d)\n", a_cnt,
			b_cnt, c_cnt, (int) a_percent, (int) b_percent, (int) c_percent);

	kprintf("\n\n########## Test Case5, ps():\n");
	ps();
	kill(49);
}

proc_basic(char c) {
	int count = 0;

	while (count++ < 1000) {
		kprintf("%c", c);
	}
}

proc_a(c)
	char c; {
	int i;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		a_cnt++;
	}
}

proc_b(c)
	char c; {
	int i;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		b_cnt++;
	}
}

proc_c(c)
	char c; {
	int i;

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
