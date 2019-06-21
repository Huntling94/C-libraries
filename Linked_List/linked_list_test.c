#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linked_list.h"
#include "..\Utilities\utils.h"

int main(void)
{
    list_t* list = create_linked_list();
    list_pushl(list, integer(5));
    list_pushl(list, integer(6));
    list_pushl(list, integer(7));
    list_pushr(list, integer(1000));
    
    list_traverse_lr(list, print_int);
    printf("Traversed left to right\n");

    print_int(list_popr(list));
    print_int(list_popr(list));
    print_int(list_popr(list));
    print_int(list_popr(list));
    printf("List is empty: "); (list_is_empty(list)) ? (printf("Yes\n")) : (printf("No, length: %d\n", list_len(list)));
    printf("All popped off\n");

    list_pushl(list, integer(6));
    list_pushl(list, integer(7));
    list_pushr(list, integer(1000));
    
    void* found = list_find(list, integer(7), int_cmp);
    (found == NULL) ? (printf("Not Found\n")) : (printf("Found: %d\n", *((int*)found)));

    list_pushl(list, integer(6));
    list_pushl(list, integer(6));
    list_pushl(list, integer(6));
    int num_found = 0;
    void** found_six = list_find_all(list, integer(6), &num_found, int_cmp);
    printf("Total Found: %d\n", num_found);
    printf("Pointer of found_six: %p\n", found_six);
    int i;
    for(i=0; i<num_found; i++){
        printf("Found: "); print_int(found_six[i]);
    }
    free(found_six);
    print_int(list_popr(list));
    print_int(list_popr(list));
    print_int(list_popr(list));
    print_int(list_popr(list));
    print_int(list_popr(list));
    print_int(list_popr(list));
    //print_int(linked_list_pop(list));
    //print_int(linked_list_pop(list));
    //print_int(linked_list_pop(list));
    printf("List is empty: "); (list_is_empty(list)) ? (printf("Yes\n")) : (printf("No\n"));
}