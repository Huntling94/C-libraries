#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "rbt.h"

static rbt_node_t nil = {0, BLACK, UNVISITED, 0, 0, 0, 0};        

#define NIL &nil

void error_message(char* msg)
{
    assert(msg != NULL);
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

void error_set_to_null_message(char* noun)
{
    assert(noun != NULL);
    fprintf(stderr, "Error: %s set to NULL (uninitialised). "
                    "Please pass initialised %s into function\n", noun, noun);
    exit(EXIT_FAILURE);
}

static void in_order_traversal(rbt_t* tree, void action(void* data));
static void post_order_traversal(rbt_t* tree, void action(void* data));
static void destroy_rbt(rbt_t* tree, void free_data(void* data));
static void rbt_insert(rbt_t* tree, void* data);
static void* rbt_get_min(rbt_t* tree);
static void* rbt_get_max(rbt_t* tree);
static void* rbt_in(rbt_t* tree, void* key);
static int rbt_len(rbt_t* tree);
void insert_fixup(rbt_t* tree, rbt_node_t* node);
static void rbt_print(rbt_t* tree, void print(void* data));

rbt_t* create_rbt(int cmp_func(const void* a, const void* b))
{
    rbt_t* ret = malloc(sizeof *ret);
    assert(unwanted_null(ret));

    ret->root = NIL;

    ret->cmp = cmp_func;
    ret->in_order_traversal = in_order_traversal;
    ret->post_order_traversal = post_order_traversal;
    ret->insert = rbt_insert;
    ret->destroy = destroy_rbt;
    ret->min = rbt_get_min;
    ret->max = rbt_get_max;
    ret->in = rbt_in;
    ret->len = rbt_len;
    ret->print = rbt_print;
    return ret;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_rbt_node
 *
 * Arguments: data node is to contain
 *            colour (RED or BLACK) of node
 *
 * Returns: pointer to the red black tree node
 *
 * Dependency: utils.h
 */
rbt_node_t* create_rbt_node(void* data, unsigned int colour)
{
    if (data == NULL){
        error_set_to_null_message("data");
    }
    if (colour != RED && colour != BLACK){
        error_message("Colour must be RED or BLACK");
    }
    rbt_node_t* ret = malloc(sizeof *ret);
    assert(unwanted_null(ret));
    
    
    ret->data = data;
    ret->colour = colour;
    ret->visited = UNVISITED;
    ret->frequency = 1;
    ret->left = NIL;
    ret->right = NIL;
    ret->parent = NIL;
    return ret;
}
//-----------------------------------------------------------------------------


/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: rbt_insert
 *
 * Arguments: red black tree
 *            data to insert into tree
 *
 * Returns: Void (inserts data into tree)
 *
 * Dependency: utils.h
 *             create_rbt_node
 *             insert_fixup
 */
static void rbt_insert(rbt_t* tree, void* data)
{
    /* Error Checks */
    if(tree == NULL){
        error_set_to_null_message("tree");
    }
    if (data == NULL){
        error_set_to_null_message("data");
    }

    rbt_node_t* inserting_node = create_rbt_node(data, RED); // Insert this
    /* First insertion into tree, colour black */
    if(tree->root == NIL){
        inserting_node->colour = BLACK;
        tree->root = inserting_node;
        return;
    }

    rbt_node_t* prev = NIL;        // parent of curr
    rbt_node_t* curr = tree->root;
    int result;                     // result of comparison
    /* Find node to insert into
     * curr will be set to NIL (we insert into here)
     * prev will be the parent of the node we insert into
     */
    while (curr != NIL){
        prev = curr;
        result = tree->cmp(curr->data, data);
        if (result == 0){
            curr->frequency++;
            return;
        }
        else if(result > 0){
            curr = curr->left;
        }
        else{
            curr = curr->right;
        }
    }

    /* Insert node */
    inserting_node->parent = prev;
    if (result > 0){
        prev->left = inserting_node;
    }
    else{
        prev->right = inserting_node;
    }
    insert_fixup(tree, inserting_node);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: in_order_traversal
 *
 * Arguments: the red black tree
 *            a function that applies action to data in node, in order
 *
 * Returns: void
 *
 * Dependency: node_action_order
 */
static void node_action_order(rbt_node_t* node, void action(void* data));

static void in_order_traversal(rbt_t* tree, void action(void* data))
{
    node_action_order(tree->root, action);
}

static void node_action_order(rbt_node_t* node, void action(void* data))
{
    if (node != NIL){
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
 * Arguments: the red black tree
 *            a function that applies action to data in node, post order
 *
 * Returns: void
 *
 * Dependency: node_action_all
 */
static void node_action_all(rbt_node_t* node, void action(void* data));

static void post_order_traversal(rbt_t* tree, void action(void* data))
{
    if (action == *****&**********&****&*****&****&*******************&**free){
        error_message("You've tried to pass the free function in, however "
                      "this will cause a memory leak. To destroy the "
                      "red black tree try calling "
                      "tree->destroy(tree, free) instead.");
    }
    node_action_all(tree->root, action);
}

static void node_action_all(rbt_node_t* node, void action(void* data))
{
    if (node != NIL){
        node_action_all(node->left, action);
        node_action_all(node->right, action);
        action(node->data);

        
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: rbt_print
 *
 * Arguments: the red black tree
 *            a function that prints data in a node
 *
 * Returns: void
 *
 */
static void rbt_print(rbt_t* tree, void print(void* data))
{
     tree->in_order_traversal(tree, print);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: destroy_rbt
 *
 * Arguments: The red black tree;
              a free function for each data in each node;
 *
 * Returns: void
 *
 * Dependency: recursive_destroy
 *
 * Note that the free function from stdlib.h can be passed in for suitable data
 */
static void recursive_destroy(rbt_node_t* node, void action(void* data)){
    if (node != NIL){
        recursive_destroy(node->left, action);
        recursive_destroy(node->right, action);
        action(node->data);
        free(node);
    }
}
static void destroy_rbt(rbt_t* tree, void free_data(void* data))
{
    recursive_destroy(tree->root, free_data);
    free(tree);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: rbt_get_min
 *
 * Arguments: The red black tree;
 *
 * Returns: data of minimum node
 *
 * Dependency: utils.h
 */
static void* rbt_get_min(rbt_t* tree)
{
    if (tree == NULL){
        error_set_to_null_message("tree");
    }
    rbt_node_t* node = tree->root;
    while(node->left != NIL){
        node = node->left;
    }
    return node->data;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: rbt_get_max
 *
 * Arguments: The red black tree;
 *
 * Returns: data of maximum node
 *
 * Dependency: utils.h
 */
static void* rbt_get_max(rbt_t* tree)
{
    if (tree == NULL){
        error_set_to_null_message("tree");
    }
    rbt_node_t* node = tree->root;
    while(node->right != NIL){
        node = node->right;
    }
    return node->data;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: rbt_in
 *
 * Arguments: The red black tree;
 *            key in data for comparison
 *
 * Returns: data of node if found, otherwise NULL
 *
 * Dependency: utils.h
 * 
 * NOTE function only works when the key is the same type as the data inserted
 */
static void* rbt_in(rbt_t* tree, void* key)
{
    /* Error Checks */
    if(tree == NULL){
        error_set_to_null_message("tree");
    }
    if (key == NULL){
        error_set_to_null_message("data");
    }

    rbt_node_t* node = tree->root;
    int result;
    while(node != NIL){
        result = tree->cmp(node->data, key);
        if(result == 0){
            return node->data;
        }
        else if (result>0){
            node = node->left;
        }
        else{
            node = node->right;
        }
    }
    return NULL;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: rbt_len
 *
 * Arguments: The red black tree;
 *
 * Returns: number of nodes in tree
 *
 * Dependency: utils.h
 *             len_rbt
 */
static int len_rbt(rbt_node_t* parent)
{
    if (parent == NIL){
        return 0;
    }
    return(len_rbt(parent->left) + 1 + len_rbt(parent->right));
}

static int rbt_len(rbt_t* tree)
{
    if(tree == NULL){
        error_set_to_null_message("tree");
    }
    return len_rbt(tree->root);
}
//-----------------------------------------------------------------------------



/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: left_rotate
 *
 * Arguments: The red black tree;
              a red black tree node
 *
 * Returns: void
 *           takes a node, and rotates itself to the left, and the node to the
 *           right of it to its parent.
 * 
 * Dependency: None
 */
void left_rotate(rbt_t* tree, rbt_node_t* x)
{
    rbt_node_t* y = x->right;
    x->right = y->left;
    /* If left of y is not NIL, then its parent is now x */
    if(y->left != NIL){
        y->left->parent = x;
    }
    y->parent = x->parent;
    /* If X is the root, then we left-rotate by making y the root */
    if (x->parent == NIL){
        tree->root = y;
    }
    /* X's parent now points to y */
    else if (x == x->parent->left){
        x->parent->left = y;
    }
    /* X's parent now points to y */
    else{
        x->parent->right = y;
    }
    y->left = x;    // left of y is now pointing to x (after rotation)
    x->parent = y;  // x's parent is now y (after rotation)
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: right_rotate
 *
 * Arguments: The red black tree;
              a red black tree node
 *
 * Returns: void
 *           takes a node, and rotates to the right, and the node to the left
 *           of it to its parent.
 * 
 * Dependency: None
 */
void right_rotate(rbt_t* tree, rbt_node_t* y)
{
    rbt_node_t* x = y->left;
    y->left = x->right;
    if(x->right != NIL){
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NIL){
        tree->root = x;
    }
    else if (y == y->parent->left){
        y->parent->left = x;
    }
    else{
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}
//-----------------------------------------------------------------------------

void insert_fixup(rbt_t* tree, rbt_node_t* node)
{
    while(node->parent->colour == RED){
        if(node->parent == node->parent->parent->left){
            rbt_node_t* y = node->parent->parent->right;
            /* if right of our parents parent is red (1 level above) */
            if(y->colour == RED){
                node->parent->colour = BLACK;       // Parent set to black
                y->colour = BLACK;                  // Right of our parents parents set to black
                node->parent->parent->colour = RED; // Parents parent set to red
                node = node->parent->parent;        // Inserted node now pointing to parents parent
            }
            /* Right of our parents parent is black (1 level above) */
            else {
                /* Node is to the right of our parent */
                if (node == node->parent->right){
                    node = node->parent;            // Inserted node now pointing to parent
                    left_rotate(tree, node);
                }
                /* Node is to the left of our parent */
                node->parent->colour = BLACK;       // Parent set to black
                node->parent->parent->colour = RED; // Parents parent set to red
                right_rotate(tree, node->parent->parent);
            }
        }
        else{ // parent is to the right of its parent
            rbt_node_t* y = node->parent->parent->left;
            if(y->colour == RED){
                node->parent->colour = BLACK;
                y->colour = BLACK;
                node->parent->parent->colour = RED;
                node = node->parent->parent;
            }
            else {
                if (node == node->parent->left){
                    node = node->parent;
                    right_rotate(tree, node);
                }
                node->parent->colour = BLACK;
                node->parent->parent->colour = RED;
                left_rotate(tree, node->parent->parent);
            }
        }
    } // end while
    tree->root->colour = BLACK;
}