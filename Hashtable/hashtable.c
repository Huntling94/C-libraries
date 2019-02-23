#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "..\Utilities\utils.h"
#include "hashtable.h"

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: hash_str
 *
 * Arguments: a void pointer to a char*
 *
 * Returns: the hash of the string using ELFHash
 */
unsigned int hash_str(void* _str)
{
    char* str = _str;
    int length = strlen(str);
    unsigned int hash = 0;
    unsigned int x    = 0;
    unsigned int i    = 0;

    for (i = 0; i < length; ++str, ++i){
       hash = (hash << 4) + (*str);
       if ((x = hash & 0xF0000000L) != 0){
          hash ^= (x >> 24);
       }
       hash &= ~x;
    }
    return hash;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: hash_int
 *
 * Arguments: a void pointer to an integer
 *
 * Returns: the hash of the integer
 */
unsigned int hash_int(void* _int)
{
    unsigned int x = *((unsigned int*)_int);
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
//-----------------------------------------------------------------------------

int hashtable_insert(hashtable_t* h, void* key, void* data);
void* hashtable_is_in(hashtable_t* h, void* data);
static int hashtable_len(hashtable_t* h);
static int hashtable_get_frequency(hashtable_t* h, void* key);
static void* hashtable_most_frequent(hashtable_t* h);
void hashtable_print_frequencies(hashtable_t* h, int max);
void destroy_hashtable(hashtable_t* h);
dict_array_t* hashtable_to_array(hashtable_t* h);

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_simple_hashtable
 *
 * Arguments: the number of buckets in the table
 *            the type of the key (user passes in defined constants)
 *             only INTEGER and STRING datatypes are supported
 *
 * Returns: a pointer to an empty hashtable
 */
hashtable_t* create_simple_hashtable(unsigned int table_size, int type)
{
    hashtable_t* table = malloc(sizeof(*table));
    assert(unwanted_null(table));
    table->table_size = table_size;
    table->unique_elements = 0;
    table->in_use = 0;
    table->type = type;
    switch (type){
        case STRING:
            table->cmp = &str_cmp;
            table->hash = &hash_str;
            table->data_size = sizeof(char*);
            break;
        case INT:
            table->cmp = &int_cmp;
            table->hash = &hash_int;
            table->data_size = sizeof(int);
            break;
        default:
            fprintf(stderr, "Type not supported\n");
            assert(0);
    }
    table->insert = &hashtable_insert;
    table->in = &hashtable_is_in;
    table->len = &hashtable_len;
    table->get_frequency = &hashtable_get_frequency;
    table->most_frequent = &hashtable_most_frequent;
    table->print_frequencies = &hashtable_print_frequencies;
    table->destroy = &destroy_hashtable;
    table->to_array = &hashtable_to_array;
    table->buckets = malloc(table_size * sizeof(*table->buckets));
    assert(unwanted_null(table->buckets));

    int i;
    /* Make each bucket 'empty' */
    for(i=0; i<table_size; i++){
        table->buckets[i].bucket_limit = 1;
        table->buckets[i].bucket_size = 0;
        table->buckets[i].data = NULL;
        table->buckets[i].frequency = NULL;
        table->buckets[i].key = NULL;
    }
    return table;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: hashtable_insert
 *
 * Arguments: the hashtable
 *            the key
 *            the data
 *          
 * Returns: 0 if new insertion; 1 if duplicate insertion
 *           On new insertion, function inserts key and data to respective
 *           arrays; will also increment frequency appropriately
 */
int hashtable_insert(hashtable_t* h, void* key, void* data)
{
    int code;
    unsigned int index =  h->hash(key) % h->table_size;

    /* First Insertion into bucket*/
    if (h->buckets[index].bucket_size == 0){
        h->buckets[index].data = malloc(sizeof(*h->buckets[index].data));
        h->buckets[index].key = malloc(sizeof(*h->buckets[index].key));
        h->buckets[index].frequency = malloc(sizeof(*h->buckets[index].frequency));
        assert(unwanted_null(h->buckets[index].data));
        assert(unwanted_null(h->buckets[index].key));
        assert(unwanted_null(h->buckets[index].frequency));
        h->buckets[index].frequency[0] = 1;
        h->in_use++;
        
        
        code = 0;
    }

    /* Need more space */
    if (h->buckets[index].bucket_size == h->buckets[index].bucket_limit){
        
        h->buckets[index].bucket_limit *= 2;
        h->buckets[index].data = realloc(h->buckets[index].data,
                                         h->buckets[index].bucket_limit * sizeof(*h->buckets[index].data));
        h->buckets[index].key = realloc(h->buckets[index].key,
                                         h->buckets[index].bucket_limit * sizeof(*h->buckets[index].key));
        h->buckets[index].frequency = realloc(h->buckets[index].frequency, 
                                              h->buckets[index].bucket_limit * sizeof(*h->buckets[index].frequency));
        assert(unwanted_null(h->buckets[index].data));
        assert(unwanted_null(h->buckets[index].key));
        assert(unwanted_null(h->buckets[index].frequency));
        h->buckets[index].frequency[h->buckets[index].bucket_size] = 1;
        code = 0;
    }

    int i;
    /* Checks for a repeated hash of the key, if so, incremenets frequency
     * frees data and returns 1;
     */
    for(i=0; i<h->buckets[index].bucket_size; i++){
        if (!h->cmp(key, h->buckets[index].key[i])){
            h->buckets[index].frequency[i]++;
            //void* old_data = h->buckets[index].data[i];
           // h->buckets[index].data[i] = data;
            free(data);
            return 1;
        }
        
    }

    /* Adds new data */
    h->buckets[index].data[h->buckets[index].bucket_size] = data;
    h->buckets[index].key[h->buckets[index].bucket_size] = key;
    h->buckets[index].frequency[h->buckets[index].bucket_size++] = 1;
    h->unique_elements++;
    return code;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: hashtable_is_in
 *
 * Arguments: the hashtable
 *            the key
 *          
 * Returns: pointer to the data if key is in dictionary, otherwise NULL
 */
void* hashtable_is_in(hashtable_t* h, void* key)
{
    unsigned int index =  h->hash(key) % h->table_size;
    int i;
    for(i=0; i<h->buckets[index].bucket_size; i++){
        if (!h->cmp(key, h->buckets[index].key[i])){
            return h->buckets[index].data[i];
        }
    }
    return NULL;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: destroy_hashtable
 *
 * Arguments: the hashtable
 *          
 * Returns: void
 */
void destroy_hashtable(hashtable_t* h)
{
    int i;
    for(i=0; i<h->table_size; i++){
        if (h->buckets[i].data){
            free(h->buckets[i].frequency);
            int j;
            for(j=0; j<h->buckets[i].bucket_size; j++){
                free(h->buckets[i].data[j]);
                free(h->buckets[i].key[j]);
            }
            free(h->buckets[i].key);
            free(h->buckets[i].data);
        } 
    }
    free(h->buckets);
    free(h);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: hashtable_to_array
 *
 * Arguments: the hashtable
 *            
 * Returns: a pointer to a dict_array_t with all the key-pair-frequency values
 *          of the dictionary. NOTE this is only a shallow copy (ie) they
 *          share the same memory location, so if you destroy the dictionary
 *          or delete an item in it, the content of the array is also modified
 */
dict_array_t* hashtable_to_array(hashtable_t* h)
{
    
    int count = 0;
    int i;
    dict_array_t* ret = malloc(sizeof(*ret));
    assert(ret != NULL);
    ret->num_elements = h->unique_elements;
    ret->frequency = malloc(ret->num_elements * sizeof(*ret->frequency));
    ret->key = malloc(ret->num_elements * sizeof(*ret->key));
    ret->data = malloc(ret->num_elements * sizeof(*ret->data));

    assert(unwanted_null(ret->frequency));
    assert(unwanted_null(ret->key));
    assert(unwanted_null(ret->data));
    
    /* For each bucket */
    for(i=0; i<h->table_size; i++){
        if (h->buckets[i].data){
            int j;
            /* For each key-data-frequency pair in bucket */
            for(j=0; j<h->buckets[i].bucket_size; j++){
                ret->key[count] = h->buckets[i].key[j];
                ret->data[count] = h->buckets[i].data[j];
                ret->frequency[count++] = h->buckets[i].frequency[j];
                if (count == h->unique_elements){
                    return ret;
                }
            }
        }
    }
    return ret;
}
//-----------------------------------------------------------------------------

void destroy_dict_array(dict_array_t* a)
{
    free(a->frequency);
    free(a->key);
    free(a->data);
    free(a);
}

void hashtable_print_frequencies(hashtable_t* h, int max)
{
    int i;
    dict_array_t* a = h->to_array(h);
    printf("%*s | %10s\n", max, "Key", "Frequency");
    for(i=0; i<a->num_elements; i++){
        switch(h->type){
            case INT:
                printf("%*d | %10d\n", max, *((int*)a->key[i]), a->frequency[i]);
                break;
            case STRING:
                printf("%*s | %10d\n", max, ((char*)a->key[i]), a->frequency[i]);
                break;
            default:
                fprintf(stderr, "This should never ever ever happen\n");
                assert(0);
        }
    }
}

static void* hashtable_most_frequent(hashtable_t* h)
{
    int i, j, count=0, current_most = 0;
    void* most = NULL;
    for(i=0; i<h->table_size; i++){
        for(j=0; j<h->buckets[i].bucket_size; j++){
            if (h->buckets[i].frequency[j] > current_most){
                current_most = h->buckets[i].frequency[j];
                most = h->buckets[i].key[j];
            }
            count++;
            if (count == h->len(h)){
            return most;
            }
        }
    }
    return most;
}

static int hashtable_get_frequency(hashtable_t* h, void* key)
{
    unsigned int index =  h->hash(key) % h->table_size;
    int i;
    for(i=0; i<h->buckets[index].bucket_size; i++){
        if (!h->cmp(key, h->buckets[index].key[i])){
            return h->buckets[index].frequency[i];
        }
    }
    return 0;
}

static int hashtable_len(hashtable_t* h)
{
    return h->unique_elements;
}

int hashtable_union_len(hashtable_t* h1, hashtable_t* h2, int type)
{
    hashtable_t* ret = create_simple_hashtable(h1->table_size, type);
    int i;
    int size;
    int small_size;
    dict_array_t* large;
    dict_array_t* small;
    if (h1->len(h1) > h2->len(h2)){
        size = h1->len(h1);
        small_size = h2->len(h2);
        large = h1->to_array(h1);
        small = h2->to_array(h2);
    }
    else{
        size = h2->len(h2);
        small_size = h1->len(h1);
        large = h2->to_array(h2);
        small = h1->to_array(h1);
    }

    for(i=0; i<size; i++){
        ret->insert(ret, large->key[i], large->data[i]);
        if (i<small_size){
            ret->insert(ret, small->key[i], small->data[i]);
        }
    }
    int to_return = ret->len(ret);
    destroy_dict_array(large);
    destroy_dict_array(small);
    ret->destroy(ret);
    return to_return;
}
