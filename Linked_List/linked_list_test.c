#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linked_list.h"
#include "..\Utilities\utils.h"

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
    
    void* found = linked_list_find(list, integer(7), int_cmp);
    (found == NULL) ? (printf("Not Found\n")) : (printf("Found: %d\n", *((int*)found)));

    linked_list_push(list, integer(6));
    linked_list_push(list, integer(6));
    linked_list_push(list, integer(6));
    int num_found = 0;
    void** found_six = linked_list_find_all(list, integer(6), &num_found, int_cmp);
    printf("Total Found: %d\n", num_found);
    printf("Pointer of found_six: %p\n", found_six);
    int i;
    for(i=0; i<num_found; i++){
        printf("Found: "); print_int(found_six[i]);
    }
    free(found_six);
    print_int(linked_list_dequeue(list));
    print_int(linked_list_dequeue(list));
    print_int(linked_list_dequeue(list));
    print_int(linked_list_dequeue(list));
    print_int(linked_list_dequeue(list));
    print_int(linked_list_dequeue(list));
    //print_int(linked_list_pop(list));
    //print_int(linked_list_pop(list));
    //print_int(linked_list_pop(list));
    printf("List is empty: "); (linked_list_is_empty(list)) ? (printf("Yes\n")) : (printf("No\n"));
}