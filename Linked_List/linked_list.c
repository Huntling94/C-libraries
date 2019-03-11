#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linked_list.h"
#include "..\Utilities\utils.h"

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_linked_list
 *
 * Arguments: data to store
 *
 * Returns: pointer to doubly linked list (with a pointer to the end)
 * 
 * Dependency: utils.h
 * 
 * NOTE as a matter of implementation, the end pointer is only accurate for the
 * first list (this is to ensure popping off the list is O(1)).
 */
list_t* create_linked_list(void* data)
{
    list_t* ret = malloc(sizeof *ret);
    assert(unwanted_null(ret));

    ret->next = NULL;
    ret->prev = NULL;
    ret->end = ret;
    ret->data = data;
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: linked_list_is_empty
 *
 * Arguments: pointer to list
 *
 * Returns: 1 if list is empty, 0 otherwise
 *           A list is deemed to be empty if the end of the list is set to NULL
 *           This only happens if you pop off a list with 1 element.
 * 
 * Dependency: utils.h
 */
int linked_list_is_empty(list_t* list)
{
    if (list == NULL){
        error_set_to_null_message("linked_list");
    }
    return list->end == NULL;
}
//-----------------------------------------------------------------------------

void pointer_prev(list_t* list)
{
    while(list){
        printf("List: %p, Prev: %p\n", list, list->prev);
        list = list->prev;
    }
}

void pointer_next(list_t* list)
{
    while(list){
        printf("List: %p, Next: %p\n", list, list->next);
        list = list->next;
    }
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: linked_list_push
 *
 * Arguments: pointer to list
 *            data to store at end of list
 *
 * Returns: void (end of list has a new node containing the data)
 * 
 * Dependency: utils.h
 *             create_linked_list
 * 
 * Complexity: (O(1))
 */
void linked_list_push(list_t* list, void* data)
{
    if (list == NULL){
        error_set_to_null_message("linked_list");
    }

    if (linked_list_is_empty(list)){
        list->data = data;
        list->next = NULL;
        list->prev = NULL;
        list->end = list;
        return;
    }

    list_t* new_node = create_linked_list(data);
    assert(unwanted_null(new_node));
    new_node->end = new_node;
    new_node->prev = list->end;
    list->end->next = new_node;
    list->end = new_node;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: linked_list_pop
 *
 * Arguments: pointer to list
 *
 * Returns: data stored at the end of the list
 * 
 * Dependency: utils.h
 * 
 * Complexity: (O(1) — achieved by not updating end in all but start of list)
 */
void* linked_list_pop(list_t* list)
{
    if (list == NULL){
        error_set_to_null_message("linked_list");
    }
    if (list->end == NULL){
        error_message("Cannot pop off an empty linked_list");
    }
    list_t* end = list->end;

    /* Start of list is end of list */
    if (end == list){
        void* ret = list->data;
        list->end = NULL;
        list->next = NULL;
        list->prev = NULL;
        return ret;
    }

    list->end = list->end->prev;
    list->end->next = NULL;
    void* ret = end->data;
    free(end);
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: linked_list_prepend
 *
 * Arguments: pointer to start of list
 *            data to be stored at new start of list
 *
 * Returns: the new start of the list with data added
 *          (Will be fixed later to return void)
 * 
 * Dependency: utils.h
 *             create_linked_list
 * 
 * Complexity: (O(1))
 */
list_t* linked_list_prepend(list_t* list, void* data)
{
    if (list == NULL){
        error_set_to_null_message("linked_list");
    }
    if (list->prev != NULL){
        error_message("Please pass in the first element of the linked list "
                      "in order to use the prepend function.");
    }

    list_t* new_node = create_linked_list(data);
    assert(unwanted_null(new_node));

    list->prev = new_node;
    new_node->next = list;
    new_node->prev = NULL;
    new_node->end = list->end;
    return new_node;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: linked_list_dequeue
 *
 * Arguments: pointer to start of list
 *
 * Returns: data stored at the start of list (the list node passed in)
 * 
 * Dependency: utils.h
 *             linked_list_pop
 * 
 * Complexity: (O(1))
 */
void* linked_list_dequeue(list_t* list)
{
    if (list == NULL){
        error_set_to_null_message("linked_list");
    }

    /* Size of list is 1 */
    if(list == list->end){
        return linked_list_pop(list);
    }
    /* Size of list is 2 */
    else if(list->next == list->end){
        void* to_ret = list->data;
        list_t* to_del = list->next;
        
        list->next = NULL;
        list->end = list;
        list->data = to_del->data;
        free(to_del);
        return to_ret;
    }
    /* Size of list greater than 2 */
    else{
        void* to_ret = list->data;
        list_t* to_del = list->next;
        assert(list->next->next != NULL); // Just in case
        list->next = list->next->next;
        list->next->prev = list;
        list->data = to_del->data;
        free(to_del);
        return to_ret;
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: linked_list_traverse_lr
 *
 * Arguments: pointer to start of list
 *            function that operates on each data in list
 *
 * Returns: Void
 *           Applies the function, from the start of the list to the end,
 *           left to right, at each data element.
 * 
 * Dependency: utils.h
 * 
 * Complexity: (O(n))
 */
void linked_list_traverse_lr(list_t* list, void func(void*))
{
    if (list == NULL){
        error_set_to_null_message("linked_list");
    }
    if (linked_list_is_empty(list)){
        return;
    }
    list_t* start = list;
    while(start){
        func(start->data);
        start->end = list->end;
        start = start->next;
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: linked_list_traverse_rl
 *
 * Arguments: pointer to start of list
 *            function that operates on each data in list
 *
 * Returns: Void
 *           Applies the function, from the end of the list to the start,
 *           right to right, at each data element.
 * 
 * Dependency: utils.h
 * 
 * Complexity: (O(n))
 */
void linked_list_traverse_rl(list_t* list, void func(void*))
{
    if (list == NULL){
        error_set_to_null_message("linked_list");
    }
    if (linked_list_is_empty(list)){
        return;
    }
    list_t* end = list->end;
    while(end){
        func(end->data);
        end->end = list->end;
        end = end->prev;
    }
}

int main(void)
{
    list_t* list = create_linked_list(integer(5));
    linked_list_push(list, integer(6));
    linked_list_push(list, integer(7));
    list = linked_list_prepend(list, integer(1000));
    
    linked_list_traverse_lr(list, print_int);
    printf("Traversed left to right\n");
    linked_list_traverse_rl(list, print_int);
    printf("Traversed right to left");
    pointer_prev(list->end);
    printf("Done\n");
    pointer_next(list);
    list_t* end_view = list->end;
    while(end_view){
        print_int(end_view->data);
        end_view = end_view->prev;
    }
    printf("Done\n");
    print_int(linked_list_pop(list));
    print_int(linked_list_pop(list));
    print_int(linked_list_pop(list));
    print_int(linked_list_pop(list));
    printf("List is empty: "); (linked_list_is_empty(list)) ? (printf("Yes\n")) : (printf("No\n"));
    printf("All popped off\n");

    linked_list_push(list, integer(6));
    linked_list_push(list, integer(7));
    list = linked_list_prepend(list, integer(1000));
    print_int(linked_list_dequeue(list));
    print_int(linked_list_dequeue(list));
    print_int(linked_list_dequeue(list));
    //print_int(linked_list_pop(list));
    //print_int(linked_list_pop(list));
    //print_int(linked_list_pop(list));
    printf("List is empty: "); (linked_list_is_empty(list)) ? (printf("Yes\n")) : (printf("No\n"));
}

