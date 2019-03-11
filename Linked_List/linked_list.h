#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct linked_list list_t;

struct linked_list{
    void* data;
    list_t* next;
    list_t* prev;
    list_t* end;
};

list_t* create_linked_list(void* data);

#endif // LINKED_LIST_H