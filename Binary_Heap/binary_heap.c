#include <stdio.h>
#include "..\Utilities\utils.h"
#include "binary_heap.h"

#define DEFAULT_ALLOC 10
static void print_heap(binary_heap_t* h, void print(void*));

/*****************************************************************************/
/****************** Private Helper Functions for Binary Heap *****************/
/*****************************************************************************/
static unsigned int parent(unsigned int i);
static unsigned int left(unsigned int i);
static unsigned int right(unsigned int i);
static void max_heapify(binary_heap_t* h, unsigned int p);
static void build_max_heap(binary_heap_t* h);

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Functions: parent, left, right
 *
 * Arguments: index of a node
 *
 * Returns: index of the nodes parent, left, or right's index respectively
 * 
 * Dependency: None
 */
static unsigned int parent(unsigned int i){
    return i/2;
}

static unsigned int left(unsigned int i){
    return i*2 + 1;
}

static unsigned int right(unsigned int i){
    return i*2 + 2;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Functions: max_heapify
 *
 * Arguments: binary_heap
 *            index of parent
 *
 * Returns: Void.
 *           Swaps maximum of {parent, left, right} into parent index.
 *           If highest is parent, terminates, otherwise recursively calls
 *           until highest is parent (guaranteed if both leaves don't exist)
 * 
 * Dependency: utils.h
 */
static void max_heapify(binary_heap_t* h, unsigned int p)
{
    unsigned int l = left(p);
    unsigned int r = right(p);
    unsigned int largest = p; // Assume parent node is largest 
    /* If left node defined and left node greater than parent, VIOLATION */
    if(l < h->len && h->cmp(h->heap[l], h->heap[p]) > 0){
        largest = l;
    }

    /* If right node defined, and right node greater than largest node, VIOLATION */
    if (r < h->len && h->cmp(h->heap[r], h->heap[largest]) > 0){
        largest = r; // Largest element now right node
    }
    /* If largest node isn't the parent, a violation has occurred, so swap */
    if (largest != p){
        scalar_swap(&h->heap[p], &h->heap[largest], sizeof(h->heap[p]));
        max_heapify(h, largest);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: build_max_heap
 *
 * Arguments: heap
 *
 * Returns: Void (converts current heap into a max heap)
 * 
 * Dependency: max_heapify
 */
static void build_max_heap(binary_heap_t* h)
{
    unsigned int i;
    for(i=(h->len)/2; i>0; i--){
        max_heapify(h, i);
    }
    max_heapify(h, 0);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/****************** Public Helper Functions for Binary Heap ******************/
/*****************************************************************************/
binary_heap_t* create_binary_heap(int n, int cmp(const void*, const void*));
binary_heap_t* array_to_binary_heap(int* A, int n, int cmp(const void*, const void*));

static void binary_heap_push(binary_heap_t* h, void* data);
static void* binary_heap_pop(binary_heap_t* h);
static int binary_heap_is_empty(binary_heap_t* h);

/*****************************************************************************/
/******************* Constructor Functions for Binary Heap *******************/
/*****************************************************************************/


binary_heap_t* create_binary_heap(int n, int cmp(const void*, const void*))
{
    if(n <= 0){
        n = DEFAULT_ALLOC;
    }

    binary_heap_t* heap = malloc(sizeof *heap);
    assert(unwanted_null(heap));
    heap->len = 0;
    heap->heap = malloc(n * sizeof(*heap->heap));
    assert(unwanted_null(heap->heap));
    heap->alloc = n;

    heap->cmp = cmp;
    heap->is_empty = binary_heap_is_empty;
    
    return heap;
}
//--------------------------------------------------------------------------

binary_heap_t* array_to_binary_heap(int* A, int n, int cmp(const void*, const void*))
{
    binary_heap_t* heap = malloc(sizeof *heap);
    assert(unwanted_null(heap));
    heap->len = n;
    heap->alloc = n;
    heap->heap = malloc(n * sizeof(*heap->heap));
    assert(unwanted_null(heap->heap));

    int i;
    for(i=0; i<n; i++){
        heap->heap[i] = integer(A[i]);
    }
    heap->cmp = cmp;
    heap->is_empty = binary_heap_is_empty;
    return heap;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/********************* Operation Functions on Binary Heap ********************/
/*****************************************************************************/

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: binary_heap_is_empty
 *
 * Arguments: binary_heap
 *
 * Returns: 1 if empty, 0 otherwise
 * 
 * Dependency: utils.h
 */
static int binary_heap_is_empty(binary_heap_t* h){
    if(h == NULL){
        error_set_to_null_message("binary_heap");
    }
    return h->len == 0;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: binary_heap_push
 *
 * Arguments: binary_heap
 *            data to be pushed on
 *
 * Returns: Void. Pushes data onto heap.
 * 
 * Dependency: utils.h
 *             build_max_heap
 */
static void binary_heap_push(binary_heap_t* h, void* data)
{
    if(h == NULL){
        error_set_to_null_message("binary_heap");
    }
    if (data == NULL){
        error_set_to_null_message("data");
    }
    h->heap[h->len++] = data;
    print_heap(h, print_int);
    build_max_heap(h);
    print_heap(h, print_int);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: binary_heap_pop
 *
 * Arguments: binary_heap
 *
 * Returns: maximum element of heap (0th index)
 * 
 * Dependency: utils.h
 *             build_max_heap
 */
static void* binary_heap_pop(binary_heap_t* h)
{
    if(h == NULL){
        error_set_to_null_message("binary_heap");
    }
    if(h->is_empty(h)){
        error_message("Cannot pop an element off an empty binary heap");
    }
    void* ret = h->heap[0];
    int i;
    for(i=0; i<h->len-1; i++){
        h->heap[i] = h->heap[i+1];
    }
    h->len--;
    build_max_heap(h);
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: binary_heap_destroy
 *
 * Arguments: binary_heap
 *
 * Returns: Void (destroys binary heap)
 * 
 * Dependency: utils.h
 *             binary_heap_is_empty
 */
static void binary_heap_destroy(binary_heap_t* h)
{
    if(h == NULL){
        error_set_to_null_message("binary_heap");
    }
    int i;
    while(!h->is_empty(h)){
        free(h->heap[--h->len]);
    }
    free(h->heap);
    free(h);
}
//-----------------------------------------------------------------------------

static void print_heap(binary_heap_t* h, void print(void*))
{
    unsigned int i;
    for(i=0; i<h->len; i++){
        printf("%d: ", i); print_int(h->heap[i]);
    }
    printf("Done\n");
}

int main(void)
{
    binary_heap_t* heap = create_binary_heap(0, int_cmp);
    binary_heap_push(heap, integer(16));
    binary_heap_push(heap, integer(4));
    binary_heap_push(heap, integer(10));
    binary_heap_push(heap, integer(14));
    binary_heap_push(heap, integer(7));
    binary_heap_push(heap, integer(9));
    binary_heap_push(heap, integer(3));
    binary_heap_push(heap, integer(2));
    binary_heap_push(heap, integer(8));
    binary_heap_push(heap, integer(1));
    printf("Done\n");
    print_heap(heap, print_int);
    printf("Testing\n");
    int B[] = {12, 15, 7, 7, 90, 2, 10, 3, 19, 24};
    //int A[] = {16, 4, 10, 14, 7, 9, 3, 2, 8, 1};
    binary_heap_t* test = array_to_binary_heap(B, sizeof(B)/sizeof(B[0]), int_cmp);

    print_heap(test, print_int);
    printf("Building Max Heap\n");
    build_max_heap(test);
    print_heap(test, print_int);
    printf("About to pop\n");
    int* item;
    while(!test->is_empty(test)){
        item = binary_heap_pop(test);
        print_int(item);
    }
    print_heap(test, print_int);
    binary_heap_destroy(test);
    printf("Destroyed\n");
}

