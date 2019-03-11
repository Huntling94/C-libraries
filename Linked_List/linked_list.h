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
void linked_list_traverse_lr(list_t* list, void func(void*));
void linked_list_traverse_rl(list_t* list, void func(void*));
void* linked_list_find(list_t* list, void* desired_data,
                       int cmp(const void* data, const void* desired))


#endif // LINKED_LIST_H