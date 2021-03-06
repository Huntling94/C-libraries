#ifndef RBT_H
#define RBT_H

#include "..\Utilities\utils.h"

#define RED 0
#define BLACK 1

#define UNVISITED 0
#define VISITED 1

#define FREE 1

typedef struct rbt_node rbt_node_t;
typedef struct rbt rbt_t;
struct rbt_node{
    void* data;
    unsigned int colour: 1;
    unsigned int visited: 1;
    int frequency;
    rbt_node_t* left;
    rbt_node_t* right;
    rbt_node_t* parent;
};

struct rbt{
    rbt_node_t* root;

    int  (*cmp)(const void*, const void*);
    void  (*in_order_traversal)(rbt_t* tree, void action(void* data));
    void  (*post_order_traversal)(rbt_t* tree, void action(void* data));

    void  (*insert)(rbt_t* tree, void* data);
    void* (*in)(rbt_t* tree, void* key);
    void* (*min)(rbt_t* tree);
    void* (*max)(rbt_t* tree);
    int   (*get_frequency)(rbt_t* tree, void* key);
    int   (*get_colour)(rbt_t* tree, void* key);
    int   (*len)(rbt_t* tree);
    void  (*print)(rbt_t* tree, void print(void* data));
    void  (*destroy)(rbt_t* tree, void free_data(void* data));
};

rbt_t* create_rbt(int cmp_func(const void*, const void*));

#endif //RBT_H