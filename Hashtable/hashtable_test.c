#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "..\Utilities\utils.h"
#include "hashtable.h"
#include <unistd.h>

#define SUCCESS_FAIL (printf("Success\n")) : (printf("Failed\n"))
#define MAX_SIZE 100

int main(void)
{
    unsigned int size = getpid() % MAX_SIZE;
    int i;

    printf("Testing create_simple_hashtable (with integers): ");
    hashtable_t* h = create_simple_hashtable(size, INT);

    (h->table_size == size && h->unique_elements == 0
     && h->type == INT && h->data_size == sizeof(int)) ? SUCCESS_FAIL;
    h->destroy(h);

    printf("Testing create_simple_hashtable (with C-strings): ");

    char* str_table[] = {"Dog", "Cat", "Bird", "Will", "CPP", "Estoppel"};
    size = sizeof(str_table) / sizeof(*str_table);
    h = create_simple_hashtable(size, STRING);

    for(i=0; i<size; i++){
        h->insert(h, copy_string(str_table[i]), str_table[i]);
    }

    if (h->table_size == size && h->unique_elements == size
     && h->type == STRING && h->data_size == sizeof(char*)){
        int success = 1;
        for (i=0; i<size; i++){
            if (h->in(h, str_table[i]) == NULL){
                success = 0;
                break;
            }
        }
        (success) ? SUCCESS_FAIL;
    }
    else{
        printf("Failed\n");
    }

    printf("Testing hashtable_len: ");
    (h->len(h) == size) ? SUCCESS_FAIL;
    h->destroy(h);

}