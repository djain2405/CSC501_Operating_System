#define DEFAULT	    0
#define RANDOMSCHED 1
#define LINUXSCHED  2

#define HTQUEUESIZE 1024
typedef struct {
        int q[HTQUEUESIZE+1];
        int first;
        int last;
        int count;
} ht_queue;

extern void ps();
extern void ht_init_queue(ht_queue *q);
extern void ht_enqueue(int priority, int x);
extern int  ht_dequeue(int qid);
extern int  getschedclass();
extern void setschedclass(int);
extern int  scheduleclass;
extern int  nepoch;
extern ht_queue q1;
extern ht_queue q2;
extern ht_queue q3;



