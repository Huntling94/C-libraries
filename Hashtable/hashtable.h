#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define FREQ 1300655506
#define INT 0
#define STRING 1
#define DOUBLE 2
#define CHAR 3
#define FLOAT 4
#define LONG 5

typedef struct hashtable hashtable_t;

typedef struct bucket bucket_t;
typedef struct dict_array dict_array_t;

struct hashtable{
    bucket_t* buckets;
    unsigned int table_size;
    unsigned int unique_elements;
    unsigned int in_use;
    int type;
    size_t data_size;

    int (*cmp)(const void*, const void*);
    unsigned int (*hash)(void*);

    int (*insert)(hashtable_t* h, void* key, void* data);
    void* (*in)(hashtable_t* h, void* key);
    int (*len)(hashtable_t* h);
    int (*get_frequency)(hashtable_t* h, void* key);
    void* (*most_frequent)(hashtable_t* h);
    void (*print_frequencies)(hashtable_t* h, int max);
    void (*destroy)(hashtable_t* h);
    dict_array_t* (*to_array)(hashtable_t* h);
};

struct bucket{
    int bucket_size;
    int bucket_limit;
    int* frequency;
    void** key;
    void** data;
};

struct dict_array{
    void** key;
    void** data;
    int* frequency;
    int num_elements;
};

hashtable_t* create_simple_hashtable(unsigned int table_size, int type);
void destroy_dict_array(dict_array_t* a);
int hashtable_union_len(hashtable_t* h1, hashtable_t* h2, int type);

#endif // DICTIONARY_H