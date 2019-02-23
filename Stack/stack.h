#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../Utilities/utils.h"

typedef struct stack stack_t;

stack_t* create_empty_stack(int size);

struct stack{
    int num_elements;
    int alloc_elements;
    void** stack;

    void (*push)(stack_t* s, void* elem);
    void* (*pop)(stack_t* s);
    int (*is_empty)(stack_t* s);
    void (*destroy)(stack_t* s, void free_func(void*));
};

#endif // STACK_H