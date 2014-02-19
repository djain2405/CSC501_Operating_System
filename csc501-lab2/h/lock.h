#ifndef _LOCK_H_
#define _LOCK_H_

#define NLOCKS 50
#define READ   0
#define WRITE  1
#define LFREE  '\03'
#define LUSED  '\04'

#define LOCK_INDEX_BITS 6


struct lentry {
    char lstate;    // The state LFREE or LUSED
    int  lnr;       // Count of readers
    int  lnw;       // Count of writers
    int  ltype;     // The current access type READ/WRITE
    int  lqhead;    // q index of head of list
    int  lqtail;    // q index of tail of list
    int  liter;     // lock iteration number

    //int lppriomax;  // Maximum process priority among all processes waiting in
                    // the locks wait queue
    //bs_ptr lprocs_bsptr; // bit structure that represents all of the processes currently
                         // holding the lock.
};

#define isbadlock(l) (l<0 || l>=NLOCKS)

extern struct lentry locks[];
extern int nextlock;
extern int lockiter;
extern unsigned long ctr1000;
extern abs(int arg);

void linit(void);
int  lcreate(void);
int  ldelete(int lockdescriptor);
int  lock(int ldes1, int type, int priority);
int  releaseall(int numlocks, int ldes1, ...);

#endif
