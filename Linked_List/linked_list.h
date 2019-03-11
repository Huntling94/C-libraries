#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct linked_list list_t;

struct linked_list{
    void* data;
    list_t* next;
    list_t* prev;
    list_t* end;
};

/* In contradistinction to my other libraries, the following functions
 * are not made as function pointers in order to conserve memory, since
 * usage of a linked list data structure necessitates the creation lists
 */

list_t* create_linked_list(void* data);
int linked_list_is_empty(list_t* list);
void linked_list_push(list_t* list, void* data);
void* linked_list_pop(list_t* list);
list_t* linked_list_prepend(list_t* list, void* data);
void* linked_list_dequeue(list_t* list);

#endif // LINKED_LIST_H