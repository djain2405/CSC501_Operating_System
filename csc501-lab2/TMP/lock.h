#ifndef _LOCK_H_
#define _LOCK_H_

#define NLOCKS 50
#define READ   0
#define WRITE  1
#define LFREE  '\03'
#define LUSED  '\04'

#define LOCK_INDEX_BITS 6


struct lentry {
    char lstate;          // The state LFREE or LUSED
    int  lnr;             // Count of readers
    int  lnw;             // Count of writers
    int  ltype;           // The current access type READ/WRITE
    int  lqhead;          // q index of head of list
    int  lqtail;          // q index of tail of list
    int  liter;           // lock iteration number
    int  lprio;           // Maximum priority among all processes waiting in the queue
    int  pidheld[NPROC];  // pids of the processes currently holding this lock
};

#define isbadlock(l) (l<0 || l>=NLOCKS)

extern struct lentry locks[];
extern int nextlock;
extern int lockiter;
extern unsigned long ctr1000;
extern abs(int arg);

extern void linit(void);
extern int  lcreate(void);
extern int  ldelete(int lockdescriptor);
extern int  lock(int ldes1, int type, int priority);
extern int  releaseall(int numlocks, int ldes1, ...);
extern void update_lprio(int lid);
extern void update_pinh(int pid);


#endif
