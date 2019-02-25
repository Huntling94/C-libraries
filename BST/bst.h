#ifndef BST_H
#define BST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../Utilities/utils.h"

typedef struct tree tree_t;
typedef struct node node_t;

void create_tree(tree_t* tree, int cmp_func(const void*, const void*));



struct node{
    void* data;
    int visited;
    int frequency;
    node_t* left;
    node_t* right;
};

struct tree{
    node_t* root;

    int (*cmp)(const void*, const void*);
    void (*insert)(tree_t* tree, void* data);
    node_t* (*find)(tree_t* t, void* key, int cmp(void*, void*));
    void (*print)(tree_t* t, void print(void* data));
    int (*in)(tree_t* t, void* key, int cmp(void*, void*));
    int (*len)(tree_t* t);
    void (*in_order_traversal)(tree_t* t, void action(void* data));
    void (*post_order_traversal)(tree_t* t, void action(void* data));
    void (*pre_order_traversal)(tree_t* t, void action(void* data));
    void (*visit)(tree_t* tree, void* key, int cmp(void*, void*));
    void (*unvisit_all)(tree_t* tree);
    void (*destroy)(tree_t* tree, void free_data(void* data));

};

#endif // BST_H