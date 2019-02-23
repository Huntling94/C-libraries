#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "utils.h"



/* Ternary macros for cmp functions */
#define ASCENDING 1 : -1    // For cmp functions and insertion order into BST
#define DESCENDING -1 : 1   // Descending order insertion

#define EPSILON 1e-7        // Error term for double comparison



/*****************************************************************************/
/**----------------------------------------------------------------------------
 * The following functions are made available for some of the basic C datatypes
 *
 * Functions: int_cmp
 *            str_cmp
 *            double_cmp
 *            char_cmp
 *
 * Arguments: a pointer to the respective datatype
 *            a pointer to the resective datatype
 *
 * Returns: 0 if equality, else, assuming ASSCENDING,
 *          1 if a > b, or -1 if b > a.
 *-----------------------------------------------------------------------------
 * Functions: print_int
 *            print_str
 *            print_double
 *            print_char
 *
 * Arguments: a pointer to the respective datatype
 *
 * Returns: void
 *-----------------------------------------------------------------------------
 * Functions: integer
 *            doub
 *            character
 *
 * Arguments: a literal of the respective datatype
 *
 * Returns: a pointer to the literal
 *
 * For example, if we want a binary search tree of integers, we insert like:
 * tree.insert(&tree, integer(5)); NOT tree.insert(&tree, 5);
 *
 */
void* integer(int a)
{
    void* to_return = malloc(sizeof(int));
    assert(unwanted_null(to_return));
    memcpy(to_return, &a, sizeof(int));
    return to_return;
}

void* doub(double a)
{
    void* to_return = malloc(sizeof(double));
    assert(unwanted_null(to_return));
    memcpy(to_return, &a, sizeof(double));
    return to_return;
}

void* character(char a)
{
    void* to_return = malloc(sizeof(char));
    assert(unwanted_null(to_return));
    memcpy(to_return, &a, sizeof(char));
    return to_return;
}

int int_cmp(const void* a, const void* b)
{
    int result = *((int*)a) == *((int*)b);
    if (result == 1){
        return 0;
    }
    else{
        return(*((int*)a) > *((int*)b)) ? ASCENDING;
    }
}

int str_cmp(const void* a, const void* b)
{
    int result = strcmp(((char*)a), ((char*)b));
    if (result == 0){
        return 0;
    }
    else{
        return (result > 0) ? ASCENDING;
    }
}

int double_cmp(const void* a, const void* b)
{
    int result = fabs (*((double*)a) - *((double*)b)) < EPSILON;
    if (result == 1){
        return 0;
    }
    else{
        return(*((double*)a) - *((double*)b) > 0.0) ? ASCENDING;
    }
}

int char_cmp(const void* a, const void*b)
{
    int result = *((char*)a) == *((char*)b);
    if (result == 1){
        return 0;
    }
    else{
        return(*((char*)a)  > *((char*)b)) ? ASCENDING;
    }
}
//-----------------------------------------------------------------------------


int qsort_int_cmp(const void* a, const void* b)
{
    const int* alpha = ((const int*)*(const void **)a);
    const int* beta = ((const int*)*(const void**)b);
    return int_cmp(alpha, beta);
}

int qsort_str_cmp(const void* a, const void* b)
{
    const char* alpha = ((const char*)*(const void **)a);
    const char* beta = ((const char*)*(const void**)b);
    return str_cmp(alpha, beta);
}

int qsort_double_cmp(const void* a, const void* b)
{
    const double* alpha = ((const double*)*(const void **)a);
    const double* beta = ((const double*)*(const void**)b);
    return double_cmp(alpha, beta);
}

int qsort_char_cmp(const void* a, const void* b)
{
    const char* alpha = ((const char*)*(const void **)a);
    const char* beta = ((const char*)*(const void**)b);
    return char_cmp(alpha, beta);
}

qsort_operator qsort_cmp(int mode)
{
    switch(mode){
        case INT: return qsort_int_cmp;
        case STRING: return qsort_str_cmp;
        case DOUBLE: return qsort_double_cmp;
        case CHAR: return qsort_char_cmp;
        default: assert(0);
    }
}
void print_int(void* integer)
{
    printf("%d\n", *(int*)integer);
}

void print_str(void* str)
{
    printf("%s\n", ((char*)str));
}

void print_double(void* doub)
{
    printf("%.8lf\n", *(double*)doub);
}

void print_char(void* character)
{
    printf("%c\n", *(char*)character);
}

void* copy_string(char* s)
{
    char* ret = (char*)malloc((strlen(s)+1) * sizeof(*ret));
    assert(unwanted_null(ret));
    strcpy(ret, s);
    return ((void*)ret);
}

int* copy_integer_array(int* A, int n){
    int* ret = (int*)malloc(n * sizeof(*ret));
    int i;
    for(i=0; i<n; i++){
        ret[i] = A[i];
    }
    return ret;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: scalar_copy
 *
 * Arguments: a void pointer whose underlying datatype is a scalar
 *            type of scalar
 *
 * Returns: pointer to a copy of the original scalar
 */
void* scalar_copy(void* scalar, int type)
{
    void* ret;
    size_t size;
    switch(type){
        case INT: size = sizeof(int*); break;
        case STRING: size = 1 + strlen((char*)scalar); break;
        case DOUBLE: size = sizeof(double*); break;
        case FLOAT: size = sizeof(float*);  break;
        default: assert(0);
    }
    ret = malloc(size);
    assert(unwanted_null(ret));
    memcpy(ret, scalar, size);
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: string_concatenate
 *
 * Arguments: a pointer to a C-string
 *            another pointer to another C-string
 *
 * Returns: a pointer to a 3rd string with s1 and s2 concatenated
 */
char* string_concatenate(char* s1, char* s2){
    size_t s1_size = strlen(s1);
    size_t s2_size = strlen(s2);
    char* ret = (char*)malloc((s1_size + s2_size + 1) * sizeof(*ret));
    assert(unwanted_null(ret));
    memcpy(ret, s1, s1_size);
    memcpy(ret+s1_size, s2, s2_size + 1);
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: lsearch
 *
 * Arguments: a pointer to a pointer of C-strings
 *            number of C-strings
 *            C-string searched for in A
 *
 * Returns: index of desired C-string if found, otherwise -1
 */
int lsearch(char** A, int n, char* key)
{
    int i;
    for(i=0; i<n; i++){
        if (!strcmp(A[i], key)){
            return i;
        }
    }
    return -1;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: unwanted_null
 *
 * Arguments: void pointer
 *
 * Returns: 0 if pointer points to NULL, otherwise 1
 * 
 * Use this function if you want to test what malloc returned, or the creation
 * of a file pointer, etc as it prints out an error message on NULL return
 */
int unwanted_null(void* test)
{
    if (test == NULL){
        fprintf(stderr, "%s\n", strerror(errno));
        return 0;
    }
    return 1;
}
//-----------------------------------------------------------------------------

int is_numeric(int type)
{
    return (type == INT ||
            type == DOUBLE ||
            type == LONG ||
            type == FLOAT);
}

double value(void* a, int type)
{
    assert(is_numeric(type));
    switch(type){
        case INT: return (double)*((int*)a);
        case DOUBLE: return *((double*)a);
        default: assert(0);
    }
    assert(0);
}

static int operation(void* a, void* b, int typea, int typeb, int operation)
{
    if (!is_numeric(typea) || !is_numeric(typeb)){
        assert(0);
    }
    if (typea == INT){
        switch(operation){
            case ADDITION: *((int*)a) += (int)(value(b, typeb) + 1e-9); return 0;
            case SUBTRACTION: *((int*)a) -= (int)(value(b, typeb) + 1e-9); return 0;
            default: fprintf(stderr, "Operation not recognised\n"); assert(0);
        }
    }
    else if (typea == DOUBLE){
        switch(operation){
            case ADDITION: *((double*)a) += value(b, typeb); return 0;
            case SUBTRACTION: *((double*)a) -= value(b, typeb); return 0;
            default: fprintf(stderr, "Operation not recognised\n"); assert(0);
        }
    }
    return 1;
}
int addition(void* a, void* b, int typea, int typeb)
{
    return operation(a, b, typea, typeb, ADDITION);
}

int subtraction(void* a, void* b, int typea, int typeb)
{
    return operation(a, b, typea, typeb, SUBTRACTION);
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: free_string_array
 *
 * Arguments: a pointer to a pointer of C-strings
 *            number of C-strings
 *
 * Returns: 0
 */
int free_string_array(char** A, int n)
{
    assert(A != NULL);
    int i;
    for(i=0; i<n; i++){
        free(A[i]);
    }
    free(A);
    return 0;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: scalar_swap
 *
 * Arguments: void pointer to an underlying scalar datatype
 *            void pointer to an underlying scalar datatype
 *            size of underlying scalar datatype (eg sizeof(long))
 *
 * Returns: void
 *           swaps bit pattern of addresses
 *           use with caution on non-scalar datatypes
 */
void scalar_swap(void* a, void* b, size_t bytes){
    void* temp = malloc(bytes);
    assert(unwanted_null(temp));
    memcpy(temp, a, bytes);
    memcpy(a, b, bytes);
    memcpy(b, temp, bytes);
    free(temp);
}