#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "..\Utilities\utils.h"
#include "hashtable.h"

int main(void)
{
    hashtable_t* table = create_simple_hashtable(100, INT);
    printf("Created hashtable with %u buckets\n", table->table_size);
    int i;
    for(i=0; i<table->table_size; i+=2){
        table->insert(table, integer(i), NULL);
        table->insert(table, integer(i), NULL);
        i++;
        table->insert(table, integer(i), NULL);
    }

    dict_array_t* table_array = table->to_array(table);


    /*qsort(table_array, n, sizeof(*table_array), qsort_cmp(INT));
    printf("\n\nQuick sorted\n");*/
    table->print_frequencies(table);
    
    printf("%d\n", table_array->num_elements);
    printf("%d of %d in use\n", table->in_use, table->table_size);
    table->destroy(table);
    destroy_dict_array(table_array);
    printf("Successfully destroyed\n");
    
    return 0;

}