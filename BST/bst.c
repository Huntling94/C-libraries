#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "bst.h"

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_tree
 *
 * Arguments: the tree;
 *            a comparison function which compares the data in the tree.
 *
 * Returns: void
 *
 * We either malloc space for the tree outside the function or pass the address
 * to the variable which is stored on the stack
 */
static void insert(tree_t* t, void* data);
static node_t* find(tree_t* t, void* key, int cmp(void*, void*));
static void print(tree_t* tree, void print(void* data));
static void destroy(tree_t* tree, void free_data(void* data));
static int in(tree_t* t, void* key, int cmp(void*, void*));
static int len(tree_t* t);
static void in_order_traversal(tree_t* tree, void action(void* data));
static void post_order_traversal(tree_t* tree, void action(void* data));
static void pre_order_traversal(tree_t* tree, void action(void* data));
static void visit(tree_t* tree, void* key, int cmp(void*, void*));
static void unvisit_all(tree_t* tree);

void create_tree(tree_t* tree, int cmp_func(void*, void*))
{
    tree->root = NULL;
    tree->cmp = cmp_func;
    tree->insert = &insert;
    tree->find = &find;
    tree->print = &print;
    tree->destroy = &destroy;
    tree->in = &in;
    tree->len = &len;
    tree->in_order_traversal = &in_order_traversal;
    tree->post_order_traversal = &post_order_traversal;
    tree->pre_order_traversal = &pre_order_traversal;
    tree->visit = &visit;
    tree->unvisit_all = &unvisit_all;

}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_node
 *
 * Arguments: data that has been dynamically allocated preferably
 *     Note that the function points an element in the struct to the data
       the user has provided, so it is preferable the data's memory footprint
       be on the heap.
 *
 * Returns: a pointer to the node (can be inserted by calling insert_tree)
 *
 */
static node_t* create_node(void* data)
{
    assert(data != NULL);

    node_t* to_return = malloc(sizeof(*to_return));
    assert(to_return != NULL);

    to_return->data = malloc(sizeof(*to_return->data));
    assert(to_return->data != NULL);

    to_return->visited = 0;
    to_return->frequency = 1;
    to_return->data = data;
    to_return->left = NULL;
    to_return->right = NULL;

    return to_return;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: insert
 *
 * Arguments: the tree
 *            data that has been dynamically allocated preferably
 *
 * Dependency: relies on create_node
 *
 * Returns: a pointer to the node (can be inserted by calling insert_tree)
 *
 */
static node_t* insert_tree(node_t* parent, node_t* new_node,
                           int cmp(void*, void*));

static void insert(tree_t* t, void* data){
    t->root = insert_tree(t->root, create_node(data), t->cmp);
}

/* Inserts a unique node into tree */
static node_t* insert_tree(node_t* parent, node_t* new_node,
                           int cmp(void*, void*))
{
    /* First insertion into parent at this level of tree */
    if (parent == NULL){
        parent = new_node;
        return parent;
    }
    int result = cmp(parent->data, new_node->data);

    /* Only unique insertions allowed */
    if (result == 0){
        parent->frequency++;
        return parent;
    }
    /* Note: ternary macros depend on the greater then equality sign */
    else if (result>0){
        parent->left = insert_tree(parent->left, new_node, cmp);
    }
    else{
        parent->right = insert_tree(parent->right, new_node, cmp);
    }
    return parent;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: find
 *
 * Arguments: the tree
 *            the key in the data of the tree searched for
 *            a comparison function between the key and the data
 *
 * Returns: a pointer to the node if found, or NULL if not found
 *
 * Debugging: Since the cmp function compares the data with the key, argument
 *            order is not always commutative.
 *
 */
node_t* find_key(node_t* node, void* key, int cmp(void* data, void* key));

node_t* find(tree_t* t, void* key, int cmp(void* data, void* key))
{
    return find_key(t->root, key, cmp);
}

node_t* find_key(node_t* node, void* key, int cmp(void* data, void* key))
{
    /* Not in Tree */
    if (node == NULL){
        return node;
    }
    int result = cmp(node->data, key);
    /* Found result */
    if (result == 0){
        return node;
    }
    else if(result > 0){
        return find_key(node->left, key, cmp);
    }
    else {
        return find_key(node->right, key, cmp);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: in
 *
 * Arguments: the tree
 *            the key in the data of the tree searched for
 *            a comparison function between the key and the data
 *
 * Dependency: relies on find
 *
 * Returns: 1 of data associated with key in tree, 0 otherwise
 *
 * Debugging: Since the cmp function compares the data with the key, argument
 *            order is not always commutative.
 *
 */
static int in(tree_t* t, void* key, int cmp(void*, void*))
{
    return (t->find(t, key, cmp) != NULL);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: len
 *
 * Arguments: the tree.
 *
 * Returns: the number of unique nodes in the tree
 *
 */
 static int len_tree(node_t* parent);

 static int len(tree_t* t){
    assert(t != NULL);
    return len_tree(t->root);
}

static int len_tree(node_t* parent)
{
    if (parent == NULL){
        return 0;
    }
    return(len_tree(parent->left) + 1 + len_tree(parent->right));
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: in_order_traversal
 *
 * Arguments: the tree
 *            a function that applies action to data in node, in order
 *
 * Returns: void
 *
 */
static void node_action_order(node_t* node, void action(void* data));

static void in_order_traversal(tree_t* tree, void action(void* data))
{
    node_action_order(tree->root, action);
}

static void node_action_order(node_t* node, void action(void* data))
{
    if (node){
        node_action_order(node->left, action);
        action(node->data);
        node_action_order(node->right, action);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: post_order_traversal
 *
 * Arguments: the tree
 *            a function that applies action to data in node, post order
 *
 * Returns: void
 *
 */
static void node_action_all(node_t* node, void action(void* data));

static void post_order_traversal(tree_t* tree, void action(void* data))
{
    node_action_all(tree->root, action);
}

static void node_action_all(node_t* node, void action(void* data))
{
    if (node){
        node_action_all(node->left, action);
        node_action_all(node->right, action);
        action(node->data);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: pre_order_traversal
 *
 * Arguments: the tree
 *            a function that applies action to data in node, post order
 *
 * Returns: void
 *
 */
static void node_action_all_pre(node_t* node, void action(void* data));

static void pre_order_traversal(tree_t* tree, void action(void* data))
{
    node_action_all_pre(tree->root, action);
}

static void node_action_all_pre(node_t* node, void action(void* data))
{
    if (node){
        action(node->data);
        node_action_all(node->left, action);
        node_action_all(node->right, action);

    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: unvisit_all
 *
 * Arguments: the tree
 *
 * Returns: void
 */
static void unvisit_all_help(node_t* node);

static void unvisit_all(tree_t* tree)
{
    unvisit_all_help(tree->root);
}

static void unvisit_all_help(node_t* node)
{
    if (node){
        node->visited = 0;
        unvisit_all_help(node->left);
        unvisit_all_help(node->right);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: visit
 *
 * Arguments: the tree
 *
 * Returns: void
 */

static void visit(tree_t* tree, void* key, int cmp(void*, void*))
{
    node_t* found = find(tree, key, cmp);
    if (found != NULL){
        found->visited = 1;
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: print
 *
 * Arguments: the tree
 *            a function that prints data in a node
 *
 * Returns: void
 *
 */
void print(tree_t* tree, void print(void* data))
{
     tree->in_order_traversal(tree, print);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: destroy
 *
 * Arguments: the tree,
              a free function for each data in each node.
 *
 * Returns: the number of unique nodes in the tree
 *
 * Dependency: post_order_traversal
 *
 * Note that the free function from stdlib.h can be passed in for suitable data
 */
static void destroy(tree_t* tree, void free_data(void* data))
{
    tree->post_order_traversal(tree, free_data);
    tree->root = NULL;
}
//-----------------------------------------------------------------------------
