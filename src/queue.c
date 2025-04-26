#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/queue.h"

static pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc){
        if(q == NULL || proc == NULL) return;

        pthread_mutex_lock(&queue_lock);
        if(q == NULL || q->size >= MAX_QUEUE_SIZE) {
                pthread_mutex_unlock(&queue_lock);
                return;
        }
        
        // back(đưa vào): đầu array
        // front(đưa ra): cuối array
        // priority-based enqueue
        int i = q->size - 1;
        // giá trị prio càng nhỏ thì ưu tiên càng cao
        while(i >= 0 && proc->priority < q->proc[i]->priority){
        q->proc[i+1] = q->proc[i];
        i--;
        }
        q->proc[i+1] = proc;
        q->size++;

        pthread_mutex_unlock(&queue_lock);
}

struct pcb_t* dequeue(struct queue_t *q) {
        if (q == NULL) return NULL;

        pthread_mutex_lock(&queue_lock);
        if (q->size == 0) {
                pthread_mutex_unlock(&queue_lock);
                return NULL;
        }
    
#ifdef MLQ_SCHED
        // chọn process có prio thấp nhất (ưu tiên cao)
        int idx = 0;
        for (int i = 1; i < q->size; ++i) {
                if(q->proc[i]->prio < q->proc[idx]->prio) 
                idx = i;
        }
#else
        // FIFO: chọn tiến trình đầu tiên
        int idx = 0;
#endif

        struct pcb_t *chosen = q->proc[idx];

        // dời các process sau về trước
        for(int i = idx; i < q->size - 1; ++i) 
        q->proc[i] = q->proc[i + 1];
        
        q->size--;

        pthread_mutex_unlock(&queue_lock);

        return chosen;
}
    