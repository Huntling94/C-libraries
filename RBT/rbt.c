#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "rbt.h"


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
void insert_fixup(rbt_t* tree, rbt_node_t* node);

rbt_t* create_rbt(int cmp_func(const void* a, const void* b))
{
    rbt_t* ret = malloc(sizeof *ret);
    assert(unwanted_null(ret));

    ret->root = NULL;

    ret->cmp = cmp_func;
    ret->in_order_traversal = in_order_traversal;
    ret->post_order_traversal = post_order_traversal;

    ret->destroy = destroy_rbt;
    return ret;
}

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
    ret->left = NULL;
    ret->right = NULL;
    ret->parent = NULL;
    return ret;
}



void rbt_insert(rbt_t* tree, void* data)
{
    printf("Inserting: %d\n", *((int*)data));
    /* Error Checks */
    if(tree == NULL){
        error_set_to_null_message("tree");
    }
    if (data == NULL){
        error_set_to_null_message("data");
    }

    rbt_node_t* inserting_node = create_rbt_node(data, RED);
    /* First insertion into tree, colour black */
    if(tree->root == NULL){
        inserting_node->colour = BLACK;
        tree->root = inserting_node;
        return;
    }
    rbt_node_t* prev = NULL;        // parent of curr
    rbt_node_t* curr = tree->root;
    int result;                     // result of comparison
    /* Find node to insert into
     * curr will be set to NULL (we insert into here)
     * prev will be the parent of the node we insert into
     */
    while (curr != NULL){
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
    /* TODO Balance RBT */
    printf("About to fix-up: %d, whose parent is: %d\n", *((int*)inserting_node->data), *((int*)inserting_node->parent->data));
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
    if (node){
        node_action_all(node->left, action);
        node_action_all(node->right, action);
        action(node->data);

        
    }
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
    if (node){
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

void left_rotate(rbt_t* tree, rbt_node_t* x)
{
    rbt_node_t* y = x->right;
    x->right = y->left;
    if(y->left != NULL){
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL){
        tree->root = y;
    }
    else if (x == x->parent->left){
        x->parent->left = y;
    }
    else{
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void right_rotate(rbt_t* tree, rbt_node_t* y)
{
    rbt_node_t* x = y->right;
    y->right = x->left;
    if(x->left != NULL){
        x->left->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL){
        tree->root = x;
    }
    else if (y == y->parent->left){
        y->parent->left = x;
    }
    else{
        y->parent->right = x;
    }
    x->left = y;
    y->parent = x;
}

void insert_fixup(rbt_t* tree, rbt_node_t* node)
{
    (node->parent->colour == RED) ? (printf("Colour of parent node is Red\n")) : printf("Colour of parent node is Black\n");
    while(node->parent->colour == RED){
        if(node->parent == node->parent->parent->left){
            rbt_node_t* y = node->parent->parent->right;
            if(y->colour == RED){
                node->parent->colour = BLACK;
                y->colour = BLACK;
                node->parent->parent->colour = RED;
                node = node->parent->parent;
            }
            else {
                if (node == node->parent->right){
                node = node->parent;
                left_rotate(tree, node);
                }
                node->parent->colour = BLACK;
                node->parent->parent->colour = RED;
                right_rotate(tree, node->parent->parent);
            }
        }
        else{
            printf("Else statement\n");
            assert(node->parent->parent != NULL);
            rbt_node_t* y = node->parent->parent->left;
            if(y->colour == RED){
                printf("If statement\n");
                node->parent->colour = BLACK;
                y->colour = BLACK;
                node->parent->parent->colour = RED;
                node = node->parent->parent;
            }
            else {
                if (node == node->parent->left){
                printf("Else If statement\n");
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
    printf("Done\n");
    tree->in_order_traversal(tree, print_int);
}
int main(void)
{
    rbt_t* tree = create_rbt(int_cmp);
    rbt_node_t* node = create_rbt_node(integer(10), RED);
    (node->colour == RED) ? (printf("Red Node\n")) : (printf("Black Node\n"));
    printf("Size of Tree: %d\n", sizeof *tree);
    rbt_insert(tree, integer(10));
    rbt_insert(tree, integer(11));
    rbt_insert(tree, integer(12));
    rbt_insert(tree, integer(5));
    rbt_insert(tree, integer(1));
    rbt_insert(tree, integer(6));
    (tree->root->colour == RED) ? (printf("Red Node\n")) : (printf("Black Node\n"));
    tree->in_order_traversal(tree, print_int);
    printf("Tree address: %p\n", tree);
    printf("Root: %p\n", tree->root);
    print_int(tree->root->data);
    tree->destroy(tree, free);
    (tree->root->colour == RED) ? (printf("Red Node\n")) : (printf("Black Node\n"));
    print_int(tree->root->data);
    printf("Root: %p\n", tree->root);
    printf("Tree address: %p\n", tree);
}