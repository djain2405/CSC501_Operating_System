#include <lab1.h> 

void ht_init_queue(ht_queue *q)
{
        q->first = 0;
        q->last = HTQUEUESIZE-1;
        q->count = 0;
}

void ht_enqueue(int priority, int x)
{
	ht_queue *q;
	if(priority >= 66 && priority <= 99) {
		q = &q1;
	}
	else if(priority >= 33 && priority <= 65) {
		q = &q2;
	}
	else {
		q = &q3;
	}

        if (q->count >= HTQUEUESIZE)
		kprintf("Warning: queue overflow enqueue x=%d\n",x);
        else {
                q->last = (q->last+1) % HTQUEUESIZE;
                q->q[ q->last ] = x;    
                q->count = q->count + 1;
        }
}

int ht_dequeue(int qid)
{
        int x;
	ht_queue *q;
	if(qid == 1) {
		q = &q1;
	}
	else if(qid == 2) {
		q = &q2;
	}
	else {
		q = &q3;
	}

        if (q->count <= 0)
		return -1;
        else {
                x = q->q[ q->first ];
                q->first = (q->first+1) % HTQUEUESIZE;
                q->count = q->count - 1;
        }

        return(x);
}
/*
int ht_peek(int qid)
{
        int x;
	ht_queue *q;
	if(qid == 1) {
		q = &q1;
	}
	else if(qid == 2) {
		q = &q2;
	}
	else {
		q = &q3;
	}

        if (q->count <= 0) 
		return -1;
        else
                x = q->q[ q->first ];

        return(x);
}
*/
