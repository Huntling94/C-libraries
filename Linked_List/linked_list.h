#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct linked_list list_t;

/* LEFT --> next --> next -->next --> RIGHT      */
/* LEFT <- prev <- prev <- prev <- prev <- RIGHT */ 

/* LEFT --> next -->next --> pushl popl */
/* pushr popr --> next -->next --> RIGHT */
list_t* create_linked_list();
int list_is_empty(list_t* list);
int list_len(list_t* list);
void list_pushl(list_t* list, void* data);
void list_pushr(list_t* list, void* data);
void* list_popl(list_t* list);
void* list_popr(list_t* list);
void list_traverse_lr(list_t* list, void func (void*));

void* list_find(list_t* list, void* desired_data,
                       int cmp(const void* data, const void* desired));

void** list_find_all(list_t* list, void* desired_data, int* num_found,
                            int cmp(const void* data, const void* desired));
#endif // LINKED_LIST_H