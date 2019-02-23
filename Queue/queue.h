#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue queue_t;

queue_t* create_empty_queue(int size);
int is_empty_queue(queue_t* queue);
void enqueue(queue_t *queue, int data);
int dequeue(queue_t *queue);
void destroy_queue(queue_t* q);
void print_queue(queue_t* queue);
void print_all(queue_t* queue);

struct queue{
    int len;
    int alloc;
    int front;
    int back;
    int* queue;

    int(*is_empty)(queue_t* queue);
    void(*enqueue)(queue_t* queue, int data);
    int(*dequeue)(queue_t* queue);
    void(*destroy)(queue_t* queue);
};

#endif // QUEUE_H