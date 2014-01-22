#include <stdio.h>

static unsigned long    *ebp;

int i;
void printtos()
{
    kprintf("\nTask 3 (printtos)\n");

    int a,b,c,d;
    a = 1, b = 3, c = 5, d = 7;
    asm("movl %ebp,ebp");

    kprintf("Before[0x%08x]: 0x%08x\n", ebp + 2, *(ebp + 2));
    kprintf("After[0x%08x]: 0x%08x\n", ebp, *ebp);

    for(i = 1; i <= 4; ++i){
	kprintf("\telement[0x%08x]: 0x%08x\n", ebp - i, *(ebp - i));
    }

}        
