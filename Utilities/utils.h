#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define INT 0
#define STRING 1
#define DOUBLE 2
#define CHAR 3
#define FLOAT 4
#define LONG 5 


#define ADDITION 0
#define SUBTRACTION 1
#define MULTIPLICATION 2
#define DIVISION 3

typedef int (*qsort_operator)(const void*, const void*);

void* integer(int);
void* doub(double);
void* character(char);

int int_cmp(const void* a, const void* b);
int str_cmp(const void* a, const void* b);
int double_cmp(const void* a, const void* b);
int char_cmp(const void* a, const void* b);
qsort_operator qsort_cmp(int mode);
void print_int(void* integer);
void print_str(void* str);
void print_double(void* doub);
void print_char(void* character);

int addition(void* a, void* b, int typea, int typeb);
int subtraction(void* a, void* b, int typea, int typeb);
char* string_concatenate(char* s1, char* s2);
int lsearch(char** A, int n, char* key);
int is_numeric(int type);
void* scalar_copy(void* scalar, int type);
void* copy_string(char* s);
int unwanted_null(void* test);
int free_string_array(char** A, int n);
void scalar_swap(void* a, void* b, size_t bytes);

void error_message(char* msg);
void error_set_to_null_message(char* noun);
#endif // UTILS_H