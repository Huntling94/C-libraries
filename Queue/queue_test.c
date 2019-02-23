#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include <unistd.h>

#define SUCCESS_FAIL (printf("Success\n")) : (printf("Failed\n"))
#define INIT_QUEUE_SIZE 10

int main(void)
{
    queue_t* q = create_empty_queue(INIT_QUEUE_SIZE);
    printf("Testing create_empty_queue: ");
    (q->len == q->front && q->front == q->back && q->back == 0
    && q->alloc == INIT_QUEUE_SIZE) ? SUCCESS_FAIL;

    printf("Testing is_empty_queue: ");
    q->enqueue(q, getpid());
    if (!is_empty_queue(q)){
        q->dequeue(q);
        (is_empty_queue(q)) ? SUCCESS_FAIL;
    }

    printf("Testing enqueue: ");
    unsigned int n = (INIT_QUEUE_SIZE * 3) / 2;
    int i;
    for(i=0; i<n; i++){
        enqueue(q, i);
    }
    int sum = 0;
    while(!q->is_empty(q)){
        sum += q->dequeue(q);
    }
    ((sum + n) == (n * (n + 1))/2) ? SUCCESS_FAIL;
    printf("Testing dequeue: Success\n");

    destroy_queue(q);
    
}