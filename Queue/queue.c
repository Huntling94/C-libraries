#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include "..\Utilities\utils.h"


/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_empty_queue
 *
 * Arguments: max number of elements in queue
 *
 * Returns: pointer to queue
 */
queue_t* create_empty_queue(int size)
{
    if (size < 0){
        fprintf(stderr, "Queue size must be positive, not %d\n", size);
        assert(0 && "Queue size negative");
    }
    queue_t* q = malloc(sizeof(*q));
    assert(unwanted_null(q));
    q->len = 0;
    q->front = 0;
    q->back = 0;
    q->alloc = size;
    q->queue = malloc(sizeof(*q->queue)*q->alloc);
    assert(unwanted_null(q->queue));

    q->is_empty = &is_empty_queue;
    q->enqueue = &enqueue;
    q->dequeue = &dequeue;
    q->destroy = &destroy_queue;
    return q;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: is_empty_queue
 *
 * Arguments: the queue
 *
 * Returns: 1 if empty, 0 otherwise.
 */
int is_empty_queue(queue_t* queue)
{
    assert(queue != NULL);
    return (queue->len == 0);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: enqueue
 *
 * Arguments: the queue
 *            data
 *
 * Returns: void
 */
void enqueue(queue_t *queue, int data)
{
    assert(queue != NULL);
    if (queue->len == queue->alloc){
        queue->alloc *= 2;
        queue->queue = realloc(queue->queue, queue->alloc*sizeof(int));
        assert(unwanted_null(queue->queue));
        if (queue->back < (queue->alloc)/2){
            int i;
            for(i=0; i<queue->back; i++){
                queue->queue[queue->alloc/2 + i] = queue->queue[i];
            }
            queue->back = queue->front + queue->alloc/2;
        }
    }

    queue->queue[queue->back++] = data;
    queue->len++;
    if (queue->back >= queue->alloc){
        queue->back = 0;
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: dequeue
 *
 * Arguments: the queue
 *
 * Returns: first element of queue: an integer
 */
int dequeue(queue_t *queue)
{
    assert(queue != NULL);
    int to_return = queue->queue[queue->front++];
    queue->len--;
    if (queue->front >= queue->alloc){
        queue->front = 0 ;
    }
    return to_return;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: destroy_queue
 *
 * Arguments: the queue
 *
 * Returns: void
 */
void destroy_queue(queue_t* q)
{
    if (q == NULL){
        return;
    }
    while(!q->is_empty(q)){
        dequeue(q);
    }
    free(q->queue);
    free(q);
    q = NULL;

}
//-----------------------------------------------------------------------------


void print_queue(queue_t* queue)
{
    assert(queue != NULL);
    int i;
    if (queue->back > queue->front){
        for(i=queue->front; i<queue->back; i++){
            printf("%d ", queue->queue[i]);
        }
    }
    else{
        for(i=queue->front; i<queue->alloc; i++){
            printf("%d ", queue->queue[i]);
        }
        for(i=0; i<queue->back; i++){
            printf("%d ", queue->queue[i]);
        }
    }

}

void print_all(queue_t* queue)
{
    int i;
    for(i=0; i<queue->alloc; i++){
        printf("%d ", queue->queue[i]);
    }
}
