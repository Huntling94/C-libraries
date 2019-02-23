#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stack.h"

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_empty_stack
 *
 * Arguments: max number of elements in stack
 *
 * Returns: pointer to stack
 */
void stack_push(stack_t* s, void* elem);
void* stack_pop(stack_t* s);
int stack_is_empty(stack_t* s);
void destroy_stack(stack_t* s, void free_func(void*));

stack_t* create_empty_stack(int size)
{
    if (size < 0){
        fprintf(stderr, "Stack size must be positive, not %d\n", size);
        assert(0 && "Stack size negative");
    }
    stack_t* s = malloc(sizeof(*s));
    assert(s != NULL);
    s->num_elements = 0;
    s->alloc_elements = size;

    s->stack = malloc(sizeof(s->stack)*s->alloc_elements);
    assert(s->stack != NULL);

    s->push = &stack_push;
    s->pop = &stack_pop;
    s->is_empty = &stack_is_empty;
    s->destroy = &destroy_stack;

    return s;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: stack_push
 *
 * Arguments: the stack
 *            pointer to element to be pushed onto stack
 *
 * Returns: void
 */
void stack_push(stack_t* s, void* elem)
{
    assert(s != NULL);
    if (s->num_elements == s->alloc_elements){
        s->alloc_elements *= 2;
        s->stack = realloc(s->stack, sizeof(void*)*s->alloc_elements);
    }
    s->stack[s->num_elements++] = elem;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: stack_pop
 *
 * Arguments: the stack
 *
 * Returns: void pointer to the item on top of the stack
 */
void* stack_pop(stack_t* s)
{
    assert(s != NULL);
    return s->stack[--s->num_elements];
}

//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: stack_is_empty
 *
 * Arguments: the stack
 *
 * Returns: 1 if num_elements is 0, and 0 otherwise
 */
int stack_is_empty(stack_t* s)
{
    return s->num_elements == 0;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: destroy_stack
 *
 * Arguments: the stack
 *            a function to free a stack element
 *             if non-dynamically allocated element, pass NULL in.
 *
 * Returns: void
 */
void destroy_stack(stack_t* s, void free_func(void*))
{
    if (free_func == NULL){
        goto end;
    }

    
    while(!stack_is_empty(s)){
        free_func(stack_pop(s));
    }
    assert(stack_is_empty(s));

    end: ;
    free(s);
}

