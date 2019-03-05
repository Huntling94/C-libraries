#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H



typedef struct binary_heap binary_heap_t;

struct binary_heap{
    void** heap;
    unsigned int len;
    unsigned int alloc;

    int   (*cmp)(const void*, const void*);
    int   (*is_empty)(binary_heap_t* heap);
    void  (*push)(binary_heap_t* heap, void* elem);
    void* (*pop)(binary_heap_t heap);
    void* (*pushpop)(binary_heap_t heap, void* elem);
};

binary_heap_t* create_binary_heap(int n, int cmp(const void*, const void*));
#endif // BINARY_HEAP_H