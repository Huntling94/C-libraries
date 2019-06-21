#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linked_list.h"
#include "..\Utilities\utils.h"

typedef struct node node_t;


struct node{
    void* data;
    node_t* next;
    node_t* prev;
};


struct linked_list{
    int len;
    node_t* left;
    node_t* right;
};

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_node
 *
 * Arguments: pointer to data to store in node
 *
 * Returns: pointer to node
 * 
 * Dependency: utils.h
 */
node_t* create_node(void* data)
{
    node_t* node = malloc(sizeof*node);
    assert(unwanted_null(node));
    memset(node, 0, sizeof(*node));
    node->data = data;
    return node;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_linked_list
 *
 * Arguments: void
 *
 * Returns: pointer to doubly linked list
 * 
 * Dependency: utils.h
 */
list_t* create_linked_list(void)
{
    list_t* ret = malloc(sizeof*ret);
    assert(unwanted_null(ret));
    memset(ret, 0, sizeof(*ret));
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: list_is_empty
 *
 * Arguments: pointer to list
 *
 * Returns: 1 if list is empty, 0 otherwise
 * 
 * Dependency: utils.h
 */
int list_is_empty(list_t* list)
{
    if (list == NULL){
        error_set_to_null_message("linked_list");
    }
    return list->len == 0;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: list_len
 *
 * Arguments: pointer to list
 *
 * Returns: number of nodes in list
 * 
 */
int list_len(list_t* list)
{
    if (list == NULL){
        error_set_to_null_message("list");
    }
    return list->len;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: list_pushl
 *
 * Arguments: pointer to list
 *            data to store at node
 *
 * Returns: void (right of list has a new node containing the data)
 * 
 * Dependency: utils.h
 *             create_node
 * 
 * Complexity: (O(1))
 */
void list_pushl(list_t* list, void* data)
{
    if (list == NULL){
        error_set_to_null_message("list");
    }
    node_t* node = create_node(data);
    
    if(list_is_empty(list)){
        list->left = list->right = node;
        node->next = node->prev = NULL;
    }
    else{
        list->right->next = node;
        node->prev = list->right;
        list->right = node;
    }
    list->len++;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: list_pushr
 *
 * Arguments: pointer to list
 *            data to store at node
 *
 * Returns: void (left of list has a new node containing the data)
 * 
 * Dependency: utils.h
 *             create_node
 * 
 * Complexity: (O(1))
 */
void list_pushr(list_t* list, void* data)
{
    if (list == NULL){
        error_set_to_null_message("list");
    }
    node_t* node = create_node(data);
    if(list_is_empty(list)){
        list->left = list->right = node;
        node->next = node->prev = NULL;
    }
    else{
        list->left->prev = node;
        node->next = list->left;
        list->left = node;
    }
    list->len++;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: list_popr
 *
 * Arguments: pointer to list
 *
 * Returns: data stored right of list
 * 
 * Dependency: utils.h
 * 
 * Complexity: (O(1)
 */
void* list_popr(list_t* list)
{
    if (list == NULL){
        error_set_to_null_message("list");
    }
    if (list_is_empty(list)){
        error_message("Cannot pop off an empty list");
    }

    void* data = list->right->data;
    if(list->len == 1){
        free(list->right);
        list->left = list->right = NULL;
    }
    else if (list->len == 2){
        free(list->right);
        list->right = list->left;
    }
    else{
        list->right = list->right->prev;
        list->right->next = NULL;
    }
    list->len--;
    return data;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: list_popl
 *
 * Arguments: pointer to list
 *
 * Returns: data stored left of list
 * 
 * Dependency: utils.h
 * 
 * Complexity: (O(1)
 */
void* list_popl(list_t* list)
{
    if (list == NULL){
        error_set_to_null_message("list");
    }
    if (list_is_empty(list)){
        error_message("Cannot pop off an empty list");
    }
    void* data = list->left->data;
    if(list->len == 1){
        free(list->left);
        list->left = list->right = NULL;
    }
    else if (list->len == 2){
        free(list->left);
        list->left = list->right;
    }
    else{
        list->left = list->left->next;
        list->left->prev = NULL;
    }
    list->len--;
    return data;
}
//-----------------------------------------------------------------------------


/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: list_traverse_lr
 *
 * Arguments: pointer to list
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
void list_traverse_lr(list_t* list, void func (void*))
{
    if (list == NULL){
        error_set_to_null_message("list");
    }
    node_t* start = list->left;
    while(start){
        func(start->data);
        start = start->next;
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: linked_list_find
 *
 * Arguments: pointer list
 *            key of desired data you want
 *            comparison function that compares data with desired_data
 *
 * Returns: Pointer to *first instance of data* (left to right) if found, or NULL
 *           Please note that your comparison function may not be commutative.
 *           If the data you insert into the list is a struct, and you want to
 *           find the first instance of data with a particular key (an element)
 *           in that struct, then the cmp function is not commutative.
 *           Hence, ensure, the left argument takes the lists data, and the right
 *           the key you are searching for
 * 
 * Dependency: utils.h
 * 
 * Complexity: (O(n))
 */
void* list_find(list_t* list, void* desired_data, int cmp(const void* data, const void* desired))
{
    if (list == NULL){
        error_set_to_null_message("list");
    }
    node_t* copy = list->left;
    while(copy){
        if (!cmp(copy->data, desired_data)){
            return copy->data;
        }
        copy = copy->next;
    }
    return NULL;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: list_find_all
 *
 * Arguments: pointer to list
 *            key of desired data you want
 *            pointer to integer which will store number of times key is found
 *            comparison function that compares data with desired_data
 *
 * Returns: Pointer to *first instance of data* (left to right) if found, or NULL
 *           Please note that your comparison function may not be commutative.
 *           See find function for more details.
 * 
 * Dependency: utils.h
 * 
 * Complexity: (O(n))
 */
void** list_find_all(list_t* list, void* desired_data, int* num_found,
                     int cmp(const void* data, const void* desired))
{
    if (list == NULL){
        error_set_to_null_message("list");
    }
    int alloc = 1;
    *num_found = 0;

    void** found = malloc(alloc * sizeof(*found));
    assert(unwanted_null(found));
    node_t* copy = list->left;

    while(copy){
        if (!cmp(copy->data, desired_data)){
            if (alloc == *num_found){
                alloc *= 2;
                found = realloc(found, alloc * sizeof(*found));
            }
            found[*num_found] = copy->data;
            (*num_found)++;
        }
        copy = copy->next;
    }
    if(*num_found == 0){
        free(found);
        return NULL;
    }
    return found;
}

#if 0

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
//-----------------------------------------------------------------------------

#endif