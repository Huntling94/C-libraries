#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "..\BST\bst.h"
#include "..\Queue\queue.h"
#include "..\Stack\stack.h"
#include "..\Math_Extended\math_extended.h"
#include "..\Matrix\matrix.h"

#define FILE_NAME "locations.txt"

enum{UNDIRECTED, DIRECTED};
#define INIT_ALLOC 60
#define MAX_GETWORD_BUFFER 250

typedef struct adj_matrix adj_matrix_t;
typedef struct v_info v_info_t;
typedef char** buff_t;

v_info_t* create_data(char* name, int index);
void create_empty_adj_matrix(adj_matrix_t* m, int directed_or_undirected);

void free_v_info(void* data);

int read_file(FILE *fp, char buff[][MAX_GETWORD_BUFFER+1]);


struct v_info{
    char* name;
    int index;
    int weight;
};

struct adj_matrix{
    int num_vertices;
    int alloc_vertices;
    tree_t* vertices;

    int is_directed_graph;
    matrix_t* matrix;

    int (*index)(adj_matrix_t* m, char* v_name);
    char* (*index_to_name)(adj_matrix_t* m, int v_index);
    int (*num_edges)(adj_matrix_t* m, char* v_name);
    double (*vertices_avg_num_edges)(adj_matrix_t* m);
    double (*vertices_avg_weight)(adj_matrix_t* m);
    stack_t* (*shorest_path)(adj_matrix_t* m, char* src, char* dest);
    int (*vertices_connected)(adj_matrix_t* m, char* v1_key, char* v2_key);
    int (*eulerian)(adj_matrix_t *m);
    int (*undirected)(adj_matrix_t *m);
    int (*trace)(adj_matrix_t* m);
};



/*****************************************************************************/
/********************** Functions Concerning the Tree ************************/
/*****************************************************************************/

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: v_info_cmp
 *
 * Arguments: void pointer that points to a v_info_t;
 *            void pointer that points to a v_info_t.
 *
 * Returns: 0 if equality, else, assuming ASSCENDING,
 *          1 if a > b, or -1 if b > a.
 *
 * Dependency: bst.h
 *             str_cmp
 */
int v_info_cmp(void* v1, void* v2){
    return (str_cmp(((v_info_t*)v1)->name,
                    ((v_info_t*)v2)->name));
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: key_cmp
 *
 * Arguments: void pointer that points to the data in a v_info_t instance
 *            void pointer that points a key (a string) for v_info_t
 *
 * Returns: 0 if equality, else, assuming ASSCENDING,
 *          1 if a > b, or -1 if b > a.
 *
 * Dependency: bst.h
 *             str_cmp
 */
int key_cmp(void* data, void* key){
    return str_cmp(((void*)((v_info_t*)data)->name), key);
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: print_v_info
 *
 * Arguments: void pointer that points to a v_info_t;
 *
 * Returns: void
 *
 * Dependency: bst.h
 *             print_str
 *             print_int
 */
void print_v_info(void* v1){
    printf("Name: "); print_str(((v_info_t*)v1)->name);
    printf("Index: "); print_int(&((v_info_t*)v1)->index);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: free_v_info
 *
 * Arguments: void pointer that points to the data in a v_info_t instance
 *
 * Returns: void
 *
 * Dependency: bst.h
 */
void free_v_info(void* data){
    free(((v_info_t*)data)->name);
}
//-----------------------------------------------------------------------------


/* Initialises an empty adjacency matrix
 * If you want your own index labels, as opposed to the default natural numbersv_info
 * you can add your own index labels such that there is a one-to-one correspondence
 * between your labels and the number of vertices. You need to also pass in a
 * comparison function and other functions for the binary search tree.
 */

/*****************************************************************************/
/************************ Adjacency Matrix Functions *************************/
/*****************************************************************************/

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_empty_adj_matrix
 *
 * Arguments: the adjacency matrix
 *
 * Returns: void
 *
 * Dependency: bst.h
 */
static int find_v_index(adj_matrix_t* m, char* v_name);
static char* find_v_name(adj_matrix_t* m, int v_index);
static int vertex_num_edges(adj_matrix_t* m, char* v_name);
double vertex_avg_num_edges(adj_matrix_t* m);
double vertex_avg_weight(adj_matrix_t* m);
stack_t* bfs(adj_matrix_t* m, char* src, char* dest);
static int are_vertices_connected(adj_matrix_t* m, char* v1_key, char* v2_key);
static int is_eulerian(adj_matrix_t* m);
static int is_undirected(adj_matrix_t* m);
static int trace(adj_matrix_t* m);

void create_empty_adj_matrix(adj_matrix_t* m, int directed_or_undirected){
    m->num_vertices = 0;
    m->alloc_vertices = INIT_ALLOC;
    m->vertices = malloc(sizeof(*m->vertices));

    m->is_directed_graph = directed_or_undirected;
    create_tree(m->vertices, v_info_cmp);
    m->matrix = create_matrix(60, 60);


    m->index = &find_v_index;
    m->index_to_name = &find_v_name;
    m->num_edges = &vertex_num_edges;
    m->vertices_avg_num_edges = &vertex_avg_num_edges;
    m->vertices_avg_weight = &vertex_avg_weight;
    m->shorest_path = &bfs;
    m->vertices_connected = &are_vertices_connected;
    m->eulerian = &is_eulerian;
    m->undirected = &is_undirected;
    m->trace = &trace;
}
//-----------------------------------------------------------------------------'



/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_data
 *
 * Arguments: name of vertex
 *            index (of the matrix) of the vertex
 *
 * Returns: pointer to the v_info_t instance created
 *
 * Dependency: bst.h
 */
v_info_t* create_data(char* v_name, int index){
    v_info_t* r = malloc(sizeof(*r));
    assert(r != NULL);
    r->index = index;
    r->name = malloc(sizeof(*r->name) * (strlen(v_name) + 1));
    assert(r->name != NULL);
    strcpy(r->name, v_name);
    return r;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: find_v_index
 *
 * Arguments: the matrix
 *            name of vertex
 *
 * Returns: index (of the matrix) of the vertex
 *
 * Dependency: bst.h
 */
static int find_v_index(adj_matrix_t* m, char* v_name){
    node_t* found = m->vertices->find(m->vertices, v_name, key_cmp);
    if (found == NULL){
        return -1;
    }
    return (((v_info_t*)found->data)->index);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: find_v_name
 *
 * Arguments: the matrix
 *            index of vertx you want name of
 *
 * Returns: index (of the matrix) of the desired vertex
 *
 * Dependency: bst.h
 */
static void find_v_name_help(node_t* parent, int v_index, int end, char** ret);

static char* find_v_name(adj_matrix_t* m, int v_index){
    assert(m != NULL && m->vertices != NULL);
    if (m->vertices->root == NULL){
        return NULL;
    }
    char* to_return = NULL;
    find_v_name_help(m->vertices->root, v_index, 0, &to_return);
    return to_return;
}
static void find_v_name_help(node_t* parent, int v_index, int end, char** ret){
    if (end){
        return;
    }
    if(parent){
        if (((v_info_t*)parent->data)->index == v_index){
            end = 1;
            *ret = ((v_info_t*)parent->data)->name;
            return;
        }
         find_v_name_help(parent->left, v_index, end, ret);
         find_v_name_help(parent->right, v_index, end, ret);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: add_vertex
 *
 * Arguments: the matrix
 *            name of vertex
 *            name of vertex (an edge connects the two vertices)
 *            weight of edge:
 *             if directed, edge bidirectional, symmetric matrix
 *             if undirected, edge unidirectional from v to e
 *
 * Returns: void
 *
 * Dependency: bst.h
 *             create_data
 *             find_v_vertxex
 */
void add_vertex(adj_matrix_t* m, char* v_name, char* e_name, int weight){
    assert(m != NULL && v_name != NULL && e_name != NULL);
    int tree_len = m->vertices->len(m->vertices);
    m->vertices->insert(m->vertices, create_data(v_name, m->num_vertices));
    int new_tree_len = m->vertices->len(m->vertices);
    if (new_tree_len > tree_len){
        m->num_vertices++;
        tree_len = new_tree_len;
    }
    m->vertices->insert(m->vertices, create_data(e_name, m->num_vertices));
    new_tree_len = m->vertices->len(m->vertices);
    if (new_tree_len > tree_len){
        m->num_vertices++;
    }
    int v_index = m->index(m, v_name);
    int e_index = m->index(m, e_name);
    if (v_index == -1 || e_index == -1){
        fprintf(stderr, "Error adding vertices: %s or %s\n", v_name, e_name);
        assert(0 && "-1 cannot be an index");
    }
    m->matrix->matrix[v_index]->vector[e_index] = weight;

    (m->is_directed_graph) ? m->matrix->matrix[e_index]->vector[v_index] = weight : weight;

}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: print_adj_matrix
 *
 * Arguments: the matrix
 *
 * Returns: void
 *
 * Dependency: bst.h
 *
 * It is not recommended prit large matrices whose dimensions are larger than
 * can be displayed
 */
void print_adj_matrix(adj_matrix_t* m){
    printf("Tree Length: %d\n", m->vertices->len(m->vertices));
    int i, j;
    for(i=0; i<m->num_vertices; i++){
        printf("\t");
        for(j=0; j<m->num_vertices; j++){
            printf("%lf ", m->matrix->matrix[i]->vector[j]);
        }
        printf("\n");
    }


}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: print_vertex_info
 *
 * Arguments: the adjacency matrix
 *
 * Returns: void
 *
 * Dependency: bst.h
 */
static void print_vertex_info(adj_matrix_t* m, char* v_name){
    int v_index = m->index(m, v_name);
    if (v_index == -1){
        printf("Vertex: %s does not exist\n", v_name);
        return;
    }
    printf("Vertex: %s (%d)\n", v_name, v_index);
    int i;
    for(i=0; i<m->num_vertices; i++){
        if (m->matrix->matrix[v_index]->vector[i]){
            if (v_index == i){
                continue;
            }
            printf("\tEdge: %s (%d)\n", m->index_to_name(m, i), i);
        }
    }
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vertex_num_edges
 *
 * Arguments: the matrix
 *            name of vertex
 *
 * Returns: number of edges a vertex connects to
 *
 * Dependency: bst.h
 *             find_v_index
 *
 * It is not recommended prit large matrices whose dimensions are larger than
 * can be displayed
 */
static int vertex_num_edges(adj_matrix_t* m, char* v_name){
    int index = m->index(m, v_name);
    int i;
    int edges = 0;
    for(i=0; i<m->num_vertices; i++){
        edges += (m->matrix->matrix[index]->vector[i] != 0);
    }
    return edges;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vertex_avg_num_edges
 *
 * Arguments: the matrix
 *
 * Returns: arithmetic mean number of edges a random vertex in the graph is
 *          connected to: sum of num of edges of each vertex / num vertices.
 *
 * Dependency: bst.h
 */
double vertex_avg_num_edges(adj_matrix_t* m){
    int i, j;
    double sum = 0.0;
    for(i=0; i<m->num_vertices; i++){
        for (j=0; j<m->num_vertices; j++){
            sum += (m->matrix->matrix[i]->vector[j] != 0);
        }
    }
    return sum/m->num_vertices;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vertex_avg_weight
 *
 * Arguments: the matrix
 *
 * Returns: arithmetic mean of sum of weights for each vertex / num vertices
 *          this is effectively the sum of each component of the matrix
 *          divided by the number of vertices (dimension of matrix)
 *
 * Dependency: bst.h
 */
double vertex_avg_weight(adj_matrix_t* m){
    int i, j;
    double sum = 0.0;
        for(i=0; i<m->num_vertices; i++){
            for (j=0; j<m->num_vertices; j++){
                sum += m->matrix->matrix[i]->vector[j];
        }
    }
    return sum/m->num_vertices;
}
//-----------------------------------------------------------------------------

void print_column_sums(adj_matrix_t* m){
    int i, j;
    double sum = 0.0;
    for(i=0; i<m->num_vertices; i++){
        for(j=0; j<m->num_vertices;j++){
            sum += m->matrix->matrix[j]->vector[i];
        }
        printf("Column: %d, sum: %lf\n", i, sum);
        sum = 0.0;
    }
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: bfs
 *
 * Arguments: the matrix
 *            source vertex
 *            destination vertex
 *
 * Returns: A pointer to a stack containing the vertex indicies of the path
 *          from the source to the destination if a path exists, or NULL.
 *
 * Dependency: bst.h
 *             stack.h
 *             queue.h
 */
stack_t* bfs(adj_matrix_t* m, char* src, char* dest){
    int start = m->index(m, src);
    int desired = m->index(m, dest);
    m->vertices->unvisit_all(m->vertices);
    queue_t* q = create_empty_queue(m->num_vertices); // Queue for BFS
    m->vertices->visit(m->vertices, src, &key_cmp);   // Visit source vertex
    enqueue(q, start);
    int prev[m->num_vertices+1];                      // Store path here

    while(!q->is_empty(q)){
        int i;
        int curr_vertex_index = dequeue(q);
        /* For each vertex */
        for(i=0; i<m->num_vertices; i++){
            int adjacent_index;
            /* If it is adjacent to the current vertex we are in*/
            if (m->matrix->matrix[curr_vertex_index]->vector[i]){
                adjacent_index = i;
                /* Done if this is the destination vertex */
                if (adjacent_index == desired){
                    prev[adjacent_index] = curr_vertex_index;
                    goto done;
                }
                /* If the adjacent vertex has already been visited, continue */
                if(m->vertices->find(m->vertices,
                                     m->index_to_name(m, adjacent_index),
                                     &key_cmp)->visited){
                    continue;
                }
                /* Unvisited adjacent vertex, visit and enqueue it. */
                m->vertices->visit(m->vertices, m->index_to_name(m, adjacent_index), &key_cmp);
                enqueue(q, adjacent_index);
                prev[adjacent_index] = curr_vertex_index;


            }
        }
    }
    /* Since we never reached our dest vertex, src does not connect to it */
    return NULL;

    done: ;
    stack_t* stack = create_empty_stack(q->len);    // Return this stack
    int backtrack = prev[desired];
    /* Push path onto stack */
    while(backtrack != start){
        stack->push(stack, m->index_to_name(m, backtrack));
        backtrack = prev[backtrack];
    }
    q->destroy(q);
    return stack;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: are_vertices_connected
 *
 * Arguments: the matrix
 *            name of vertex
 *            name of vertex
 *
 * Returns: returns 1 if connected, otherwise 0;
 *
 * Dependency: bst.h
 *             find_v_index
 *
 * It is not recommended print large matrices whose dimensions are larger than
 * can be displayed
 */
static int are_vertices_connected(adj_matrix_t* m, char* v1_key, char* v2_key){
    assert(m != NULL);
    int v1_index = find_v_index(m, v1_key);
    int v2_index = find_v_index(m, v2_key);
    if (v1_index == -1 || v2_index == -1){
        return 0;
    }
    int w1 = m->matrix->matrix[v1_index]->vector[v2_index];
    int w2 = m->matrix->matrix[v2_index]->vector[v1_index];
    return (w1 && w2);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: is_eulerian
 *
 * Arguments: the matrix
 *
 * Returns: returns 1 if eulerian, otherwise 0;
 *
 * Dependency: bst.h
 */
static int is_eulerian(adj_matrix_t* m){
    int i, j;
    int edges = 0;
    for(i=0; i<m->num_vertices; i++){
        for(j=0; j<m->num_vertices; j++){
            edges += (m->matrix->matrix[i]->vector[j] && 1);
        }
        if (edges%2 != 0){
            return 0;
        }
    }
    return 1;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: is_undirected
 *
 * Arguments: the matrix
 *
 * Returns: returns 1 if eulerian, otherwise 0;
 *
 * Dependency: bst.h
 */
static int is_undirected(adj_matrix_t* m){
    int i, j;
    for(i=0; i<m->num_vertices; i++){
        for(j=0; j<i+1; j++){
            if (m->matrix->matrix[i]->vector[j] &&
                m->matrix->matrix[j]->vector[i]){
                continue;
            }
            else{
                return 0;
            }
        }
    }
    return 1;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: trace
 *
 * Arguments: the matrix
 *
 * Returns: returns the trace of the matrix (sum of main diagonal);
 *
 * Dependency: bst.h
 *             find_v_index
 *
 * It is not recommended prit large matrices whose dimensions are larger than
 * can be displayed
 */
static double dp(adj_matrix_t* m, char* v1, char* v2){
    //double inner_product = 0.0;
    int v1_index = m->index(m, v1);
    int v2_index = m->index(m, v2);
    printf("Index: %d, %d\n", v1_index, v2_index);
    return 0;
    //return dot_product(m->matrix[v1_index], m->matrix[v2_index], sizeof(double), m->num_vertices);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: trace
 *
 * Arguments: the matrix
 *
 * Returns: returns the trace of the matrix (sum of main diagonal);
 *
 * Dependency: bst.h
 *             find_v_index
 *
 * It is not recommended prit large matrices whose dimensions are larger than
 * can be displayed
 */
static int trace(adj_matrix_t* m){
    int i;
    int sum = 0;
    for(i=0; i<m->num_vertices; i++){
        if (sum + m->matrix->matrix[i]->vector[i] < sum){
            assert(0 && "Trace Integer Overflow");
        }
        sum += m->matrix->matrix[i]->vector[i];
    }
    return sum;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: num_edges
 *
 * Arguments: the matrix
 *
 * Returns: returns the number of edges in the graph (assuming undirected)
 *
 * Dependency: bst.h
 */

int graph_num_edges(adj_matrix_t* m){
    int sum = 0;
    int i, j;
    for(i=0; i<m->num_vertices; i++){
        for(j=i; j<m->num_vertices; j++){
            sum += (m->matrix->matrix[i]->vector[j]);
        }
    }
    return sum;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: modularity
 *
 * Arguments: the matrix
 *
 * Returns: the modularity (Q) of the graph/matrix
 *
 *           Q = (1/2m) sum of [(A_ij - (k_i * k_j)/2m) * delta(C_i, C_j)]
 *           A is the adjacencey matrix
 *           k_i, k_j is the degree of nodes i and j respectively.
 *           m is the number of edges in the matrix
 *           C_i, C_j is the community of node i and j respectively.
 *           delta is Kronecker's Delta, where 1 ==> i and j are in the same
 *              community, and 0 if they are in different communities
 *
 * Dependency: bst.h
 *             find_v_index
 *
 */
double modularity(adj_matrix_t* m){
    double sum = 0.0;
    int n_edges = graph_num_edges(m);
    printf("Number of Edges: %d\n", n_edges);
    int i, j;
    for(i=0; i<m->num_vertices; i++){
        char* i_name = m->index_to_name(m, i);
        for(j=0; j<m->num_vertices; j++){
            char* j_name = m->index_to_name(m, j);
            double rhs = ((m->num_edges(m, i_name) * m->num_edges(m, j_name))/(2*n_edges));
            sum += m->matrix->matrix[i]->vector[j] - rhs;
        }
    }
    printf("Sum: %lf\n", sum);
    return ((sum/(2*n_edges)));
}

int main(int argc, char* argv[]){
    adj_matrix_t m;
    create_empty_adj_matrix(&m, DIRECTED);
    printf("Created\n");
    char buff[100][MAX_GETWORD_BUFFER+1];
    FILE *fp = fopen(FILE_NAME, "r");
    int lines = read_file(fp, buff);


    int j, i=0;

    char delim[2] = "-";

    for(j=0; j<lines; j++){
        char* token;
        token = strtok(buff[j], delim);
        i=0;
        char first[51];
        while (token != NULL){
            if(i++==0){
                strcpy(first, token);
            }
            /* Do not add edge to itself */
            if(i>1){
                add_vertex(&m, first, token, 1);
            }
            token = strtok(NULL, delim);
        }

    }
    //print_adj_matrix(&m);
    m.vertices->print(m.vertices, print_v_info);
    //printf("Size: %d\n", m.vertices->len(&m.vertices));
    printf("Via Fluvial has %d connections\n", m.num_edges(&m, "Via Fluvial"));
    printf("Trace of matrix: %d\n", trace(&m));
    printf("Lago Celeste and Ferrucio Junction Connection Status: ");
    (m.vertices_connected(&m, "Lago Celeste", "Ferrucio Junction")) ? printf("Connected\n") : printf("Not Connected\n");
    printf("The graph is "); (is_eulerian(&m)) ? printf("Eulerian\n") : printf("not Eulerian\n");
    printf("The graph is "); (is_undirected(&m)) ? printf("Undirected\n") : printf("Directed\n");
    //printf("Dot product: %.50lf\n", dp(&m, "Abertal", "Via Fluvial"));
    //double A[] = {1, 1};
    //double B[] = {1, 1};
    //printf("Dot product: %.50lf\n", dot_product(A, B, sizeof(double), 2));
    print_vertex_info(&m, "Lago Celeste");
    printf("Average number of edges: %lf\n", m.vertices_avg_num_edges(&m));
    printf("Average weight per vertex: %lf\n", m.vertices_avg_weight(&m));
    stack_t* path = m.shorest_path(&m, "Reboldeaux", "Port Coimbra");
    while(!path->is_empty(path)){
        printf("%s\n", ((char*)path->pop(path)));
    }
    double mod = modularity(&m);
    printf("Modularity: %lf\n", mod);
    printf("Number of Vertices: %d\n", m.num_vertices);
    printf("Rank: %d\n", m.matrix->rank(m.matrix));

    //printf("Determinant: %lf\n", m.matrix->determinant(m.matrix));
    m.vertices->destroy(m.vertices, free_v_info);
    printf("Destroyed\n");
    m.vertices->print(m.vertices, print_v_info);




}

int
read_file(FILE *fp, char buff[][MAX_GETWORD_BUFFER+1]) {
	int c;          // Character read in
	int lines=0;    // Lines of file
	int i = 0;      // Characters per line
	while ((c = getc(fp)) != EOF) {
        if (i >= MAX_GETWORD_BUFFER){
            fprintf(stderr, "Line: %d exceeds the getword buffer of %d\n", lines, MAX_GETWORD_BUFFER);
            exit(EXIT_FAILURE);
        }
        buff[lines][i++] = c;
        /* Add null byte to close string for line */
        if (c == '\n') {
            buff[lines++][i-1] = '\0';
            i = 0;
        }
	}

	fclose(fp); // File was opened in main
    //fprintf(stderr, "\tFile read and closed successfully: %d lines read\n", lines);
	return lines;
}





