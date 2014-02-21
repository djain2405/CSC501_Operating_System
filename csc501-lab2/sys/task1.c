#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <sem.h>

#define DEFAULT_LOCK_PRIO 20

void lrd (char *msg, int lck, int lprio, int sleeptime)
{
    int     ret;

    kprintf ("  %s: to acquire lock\n", msg);
    ret = lock (lck, READ, lprio);
    if (ret == SYSERR) {
        kprintf ("  %s: lock returned SYSERR\n", msg);
        return;
    }
    if (ret == DELETED) {
        kprintf ("  %s: lock was DELETED\n", msg);
        return;
    }
    kprintf ("  %s: acquired lock, sleep 3s\n", msg);
    sleep (sleeptime);
    kprintf ("  %s: to release lock\n", msg);
    releaseall (1, lck);
}

void lwr (char *msg, int lck, int lprio, int sleeptime)
{
    int ret;
    kprintf ("  %s: to acquire lock\n", msg);
    ret = lock (lck, WRITE, lprio);
    if (ret == SYSERR) {
        kprintf ("  %s: lock returned SYSERR\n", msg);
        return;
    }
    if (ret == DELETED) {
        kprintf ("  %s: lock was DELETED\n", msg);
        return;
    }
    kprintf ("  %s: acquired lock, sleep 3s\n", msg);
    sleep (sleeptime);
    kprintf ("  %s: to release lock\n", msg);
    releaseall (1, lck);
}

void srw(char *msg, int sem, int sleeptime) 
{
    int ret;
    kprintf ("  %s: to acquire sem\n", msg);
    ret = wait(sem);
    if (ret == SYSERR) {
        kprintf ("  %s: sem returned SYSERR\n", msg);
        return;
    }
    kprintf ("  %s: acquired sem, sleep %ds\n", msg, sleeptime);
    sleep(sleeptime);
    kprintf ("  %s: to release sem\n", msg);
    signal(sem);
}


void dowait(int sleeptime) {
    kprintf("dowait Started\n");
    sleep(sleeptime);
    kprintf("dowait Ended\n");
}

int main()
{
    int lck, lrdm, lwrl, lwrh, sem, srdm, swrl, swrh, lloop, sloop;
    
    kprintf("Start priority inversion test with lock\n");

    lck  = lcreate();
    lloop = create(dowait, 2000, 25, "dowait", 1, 5);
    lwrh = create(lwr, 2000, 30, "writer", 4, "writer H", lck, 20, 2);
    lwrl = create(lwr, 2000, 10, "writer", 4, "writer L", lck, 20, 5);
    lrdm = create(lrd, 2000, 20, "reader", 4, "reader M", lck, 20, 1);

    kprintf("Start Writer L, then sleep 1s\n");
    resume(lwrl);
    sleep(1);


    kprintf("Start Writer H, then sleep 1s\n");
    resume(lwrh);
    sleep(1);
    
    kprintf("Start dowait, then sleep 1s\n");
    resume(lloop);
    sleep(1);

    sleep(10);

    kprintf ("Lock test finished!\n");

    kprintf("Start priority inversion test with semaphore\n");
    
    sem  = screate(1);
    sloop= create(dowait, 2000, 20, "dowait", 1, 5);
    swrh = create(srw, 2000, 30, "writer", 3, "writer H", sem, 5);
    swrl = create(srw, 2000, 10, "writer", 3, "writer L", sem, 3);
    srdm = create(srw, 2000, 40, "reader", 3, "reader M", sem, 1);


    kprintf("Start Writer L, then sleep 1s\n");
    resume(swrl);
    sleep(1);


    kprintf("Start Writer H, then sleep 1s\n");
    resume(swrh);
    sleep(1);

    kprintf("Start dowait, then sleep 1s\n");
    resume(sloop);
    sleep(1);
    
    
    sleep(10);

    kprintf ("Semaphore test finished!\n");




    return 0;


}
