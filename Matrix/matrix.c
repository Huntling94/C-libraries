#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include "../Vector/vector.h"
#include "matrix.h"
#include "../Math_Extended/math_extended.h"
#include "../Files/files.h"
#include "../Utilities/utils.h"





/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_matrix
 *
 * Arguments: the matrix
 *            number of rows in the matrix
 *            number of columns in the matrix
 *
 * Returns: a pointer to the matrix
 *
 * Dependency: vector.h
 */
static void print_matrix(matrix_t* m);
static void matrix_print_head(matrix_t*m, int rows);
static double matrix_trace(matrix_t* m);
static int matrix_rank(matrix_t* m);
static double matrix_determinant(matrix_t* m);
static double matrix_grand_sum(matrix_t* m);
static void destroy_matrix(matrix_t* m);
static void matrix_add_function_pointers(matrix_t* m);
static void matrix_row_swap(matrix_t* m, int row_a, int row_b);
static matrix_t* matrix_transpose(matrix_t* m);
static double get_matrix_entry(matrix_t* m, int i, int j);
static void set_matrix_entry(matrix_t* m, int i, int j, double entry);
static void set_matrix_row(matrix_t* m, double* src, int n, int row_num);
static int matrix_is_square(matrix_t* m);
static matrix_t* matrix_pow(matrix_t* m, int exponent);
static int gaussian_elimination(matrix_t* m);
static void matrix_impute_missing_values(matrix_t* m, int mode);
static double matrix_column_mean(matrix_t* m, int col_num);
static void matrix_to_csv(matrix_t* m, char* fname);


matrix_t* create_matrix(int rows, int columns)
{
    matrix_t* m = malloc(sizeof(*m));
    m->index_int = malloc(rows*sizeof(*m->index_int));
    m->index_str = malloc(rows*sizeof(*m->index_str));
    m->column_names = malloc(rows*sizeof(*m->column_names));
    
    assert(unwanted_null(m));
    assert(unwanted_null(m->index_str));
    assert(unwanted_null(m->index_int));
    assert(unwanted_null(m->column_names));

    int i;
    for(i=0; i<rows; i++){
        m->index_int[i] = i;
        m->index_str[i] = malloc(sizeof(*m->index_str[i]));
        m->column_names[i] = malloc(sizeof(*m->column_names[i]));
        assert(unwanted_null(m->index_str[i]));
        assert(unwanted_null(m->column_names[i]));

        m->index_str[i][0] = '\0';
        m->column_names[i][0] = '\0';
    }
    m->str_index_used = 0;
    m->column_index_used = 0;
    m->num_rows = rows;
    m->num_columns = columns;
    m->alloc_columns = columns;
    m->alloc_rows = rows;
    m->matrix = malloc(m->alloc_rows*sizeof(*m->matrix));
    for(i=0; i<m->alloc_rows; i++){
        m->matrix[i] = create_zero_vector(m->alloc_columns);
    }

    matrix_add_function_pointers(m);
    return m;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: clone_matrix
 *
 * Arguments: the matrix to be cloned
 *
 * Returns: pointer to a matrix with components equal to the source matrix
 *
 * Dependency: vector.h
 */
matrix_t* clone_matrix(matrix_t* m)
{
    matrix_t* dest = malloc(sizeof(*m));
    dest->index_int = malloc(m->num_rows*sizeof(*dest->index_int));
    dest->index_str = malloc(m->num_rows*sizeof(*dest->index_str));
    dest->column_names = malloc(m->num_rows*sizeof(*dest->column_names));

    assert(unwanted_null(dest));
    assert(unwanted_null(dest->index_str));
    assert(unwanted_null(dest->index_int));
    assert(unwanted_null(dest->column_names));
    int i;
    for(i=0; i<m->num_rows; i++){
        dest->index_str[i] = malloc((strlen(m->index_str[i])+1)*sizeof(*m->index_str[i]));
        dest->column_names[i] = malloc((strlen(m->column_names[i])+1)*sizeof(*m->column_names[i]));
        
        assert(unwanted_null(dest->index_str[i]));
        assert(unwanted_null(dest->column_names[i]));
        strcpy(dest->index_str[i], m->index_str[i]);
        strcpy(dest->column_names[i], m->column_names[i]);
    }
    memcpy(dest->index_int, m->index_int, m->num_rows*sizeof(*m->index_int));
    dest->str_index_used = m->str_index_used;
    dest->str_index_used = m->column_index_used;
    dest->num_rows = m->num_rows;
    dest->num_columns = m->num_columns;
    dest->alloc_columns = m->num_columns;
    dest->alloc_rows = m->num_rows;
    dest->matrix = malloc(m->alloc_rows*sizeof(*dest->matrix));
    assert(unwanted_null(dest->matrix));

    for(i=0; i<dest->alloc_rows; i++){
        dest->matrix[i] = create_vector_from_array(m->matrix[i]->vector,
                                                   m->matrix[i]->dimension);
    }
    matrix_add_function_pointers(dest);
    return dest;
}
//-----------------------------------------------------------------------------

static void matrix_add_function_pointers(matrix_t* m)
{
    m->print = &print_matrix;
    m->print_head = &matrix_print_head;
    m->trace = &matrix_trace;
    m->rank = &matrix_rank;
    m->determinant = &matrix_determinant;
    m->grand_sum = &matrix_grand_sum;
    m->transpose = &matrix_transpose;
    m->free = &destroy_matrix;
    m->copy = &clone_matrix;
    m->row_swap = &matrix_row_swap;
    m->get_entry = &get_matrix_entry;
    m->set_entry = &set_matrix_entry;
    m->set_matrix_row = &set_matrix_row;
    m->is_square = &matrix_is_square;
    m->pow = &matrix_pow;
    m->gaussian_elimination = &gaussian_elimination;
    m->impute_missing_values = &matrix_impute_missing_values;
    m->matrix_column_mean =  &matrix_column_mean;
    m->to_csv = &matrix_to_csv;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_addition
 *
 * Arguments: matrix 1
 *            matrix 2
 *             (must be same size)
 *
 * Returns: A third matrix whose components are the sum of the components
 *          of the first two matrices
 *
 * Dependency: clone_matrix
 *             vector.h
 */
matrix_t* matrix_addition(matrix_t* m1, matrix_t* m2)
{
    assert(m1 != NULL && m2 != NULL);
    assert(m1->num_columns == m2->num_columns && m1->num_rows == m2->num_rows);
    matrix_t* m3 = clone_matrix(m1);
    int i;
    for(i=0; i<m3->num_rows; i++){
        vector_t* temp = m3->matrix[i];
        m3->matrix[i] = vector_addition(m3->matrix[i], m2->matrix[i]);
        temp->free(temp);
    }
    return m3;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_scalar_multiplication
 *
 * Arguments: matrix 1
 *            scalar
 *
 * Returns: A matrix whose components are a scalar multiple of the components
 *          of the first matrix
 *
 * Dependency: clone_matrix
 *             vector.h
 */
matrix_t* matrix_scalar_multiplication(matrix_t* m1, double scalar)
{
    assert(m1 != NULL);
    matrix_t* ret = clone_matrix(m1);
    int i, j;
    for(i=0; i<ret->num_rows; i++){
        for(j=0; j<ret->num_columns; j++){
            ret->matrix[i]->vector[j] *= scalar;
        }
    }
    return ret;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: get_matrix_entry
 *
 * Arguments: matrix
 *            ith component
 *            jth component
 *             (starting from 0)
 *
 * Returns: the entry in the ij slot.
 */
static double get_matrix_entry(matrix_t* m, int i, int j)
{
    assert(m != NULL);
    assert(m->num_rows > i && m->num_columns > j);
    return (m->matrix[i]->vector[j]);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: set_matrix_entry
 *
 * Arguments: matrix
 *            ith component
 *            jth component
 *            entry to be set into ij slot
 *
 * Returns: void
 */
static void set_matrix_entry(matrix_t* m, int i, int j, double entry)
{
    assert(m != NULL);
    assert(m->num_rows > i);
    assert(m->num_columns > j);
    m->matrix[i]->set(m->matrix[i], j, entry);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: set_matrix_row
 *
 * Arguments: matrix
 *            array of doubles that will form a row vector in the matrix
 *            size of array of doubles
 *            row number of matrix to be set
 *
 * Returns: void
 *           sets row number in matrix to be the array of doubles
 */
static void set_matrix_row(matrix_t* m, double* src, int n, int row_num)
{
    assert(m != NULL);
    assert(m->num_rows > row_num);
    assert(m->num_columns == n);
    vector_t* temp = m->matrix[row_num];
    m->matrix[row_num] = create_vector_from_array(src, n);
    temp->free(temp);
}
//-----------------------------------------------------------------------------

void matrix_set_column(matrix_t* m, double* src, int n, int column_num)
{
    ;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_trace
 *
 * Arguments: matrix
 *
 * Returns: the trace of a matrix if it is square, otherwise asserts(0)
 */
static double matrix_trace(matrix_t* m)
{
    assert(m != NULL && m->num_columns == m->num_rows);
    double sum = 0.0;
    int i;
    for(i=0; i<m->num_rows; i++){
        sum += m->matrix[i]->vector[i];
    }
    return sum;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_row_swap
 *
 * Arguments: matrix
 *            row_a to swap
 *            row_b to swap
 *
 * Returns: void
 *           swaps the two vectors in the matrix
 */
static void matrix_row_swap(matrix_t* m, int row_a, int row_b)
{
    assert(m != NULL);
    assert(m->num_rows > row_a && "Row out of range");
    assert(m->num_rows > row_b && "Row out of range");
    vector_t* temp = m->matrix[row_a];
    m->matrix[row_a] = m->matrix[row_b];
    m->matrix[row_b] = temp;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_transpose
 *
 * Arguments: matrix
 *
 * Returns: a pointer to a matrix that is the transpose of the original
 */
static matrix_t* matrix_transpose(matrix_t* m)
{
    matrix_t* ret = create_matrix(m->num_columns, m->num_rows);
    int i, j;
    for(i=0; i<m->num_rows; i++){
        for(j=0; j<m->num_columns; j++){
            ret->matrix[j]->vector[i] = m->matrix[i]->vector[j];
        }
    }
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_multiply
 *
 * Arguments: matrix 1
 *            matrix 2
 *
 * Returns: pointer to resultant matrix if the matrices commute, otherwise NULL
 *
 * Let m1 = A be an n x m matrix, and let m2 = B be an m x p matrix.
 * Then let m1 x m2 = C be an n x p matrix, where C_ij = the sum of
 * A_ik dotproduct with B_kj.
 */
matrix_t* matrix_multiply(matrix_t* m1, matrix_t* m2)
{
    assert(m1 != NULL && m2 != NULL);
    if (m1->num_columns != m2->num_rows){
        return NULL;
    }
    matrix_t* ret = create_matrix(m1->num_rows, m2->num_columns);
    int i, j, k;
    for(i=0; i<ret->num_rows; i++){
        for(j=0; j<ret->num_columns; j++){
            double tmp = 0.0;   // Stores dotproduct
            for(k=0; k<ret->num_rows; k++){
                /* A_ik x B_kj = C_ij */
                tmp += get_matrix_entry(m1, i, k) * get_matrix_entry(m2, k, j);
            }
            ret->matrix[i]->vector[j] = tmp;
        }
    }
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_hadamard_product
 *
 * Arguments: matrix 1
 *            matrix 2
 *
 * Returns: pointer to resultant matrix if same size, otherwise NULL
 *
 * Dependency: create_matrix
 *             vector.h
 */
matrix_t* matrix_hadamard_product(matrix_t* m1, matrix_t* m2)
{
    assert(m1 != NULL && m2 != NULL);
    if (m1->num_columns != m1->num_rows
        || m1->num_columns != m2->num_columns
        || m1->num_rows != m2->num_rows){
            return NULL;
        }

    matrix_t* ret = create_matrix(m1->num_rows, m1->num_columns);
    int i;
    for(i=0; i<ret->num_rows; i++){
        vector_t* temp = ret->matrix[i];
        ret->matrix[i] = vector_hadamard_product(m1->matrix[i], m2->matrix[i]);
        temp->free(temp);
    }
    return ret;
}
//-----------------------------------------------------------------------------


static void print_matrix(matrix_t* m)
{
    int i;
    printf("%d x %d Matrix\n", m->num_rows, m->num_columns);
    for(i=0; i<m->num_rows; i++){
        if (m->str_index_used){
            printf("%40s: ", m->index_str[i]);
        }
        m->matrix[i]->print(m->matrix[i]);
    }
    printf("\n");
}

static void matrix_print_head(matrix_t*m, int rows)
{
    int i;
    printf("%d x %d Matrix\n", m->num_rows, m->num_columns);
    for (i=0; i<rows; i++){
        if (m->str_index_used){
            printf("%40s: ", m->index_str[i]);
        }
        m->matrix[i]->print(m->matrix[i]);
    }
    printf("\n");
}

void print_index(matrix_t* m){
    int i;
    for(i=0; i<m->num_rows; i++){
        printf("%d ", m->index_int[i]);
    }
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: destroy_matrix
 *
 * Arguments: matrix
 *
 * Returns: void
 *
 * Dependency: "vector.h"
 */
static void destroy_matrix(matrix_t* m)
{
    assert(m != NULL);
    int i;
    for(i=0; i<m->num_rows; i++){
        m->matrix[i]->free(m->matrix[i]);
        free(m->index_str[i]);
        free(m->column_names[i]);
    }
    free(m->index_int);
    free(m->column_names);
    free(m->index_str);
    free(m->matrix);
    free(m);
    m = NULL;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_is_square
 *
 * Arguments: matrix
 *
 * Returns: 1 if matrix is a square matrix, 0 otherwise
 */
static int matrix_is_square(matrix_t* m)
{
    assert(m != NULL);
    return (m->num_rows == m->num_columns);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_grand_sum
 *
 * Arguments: matrix
 *
 * Returns: the sum of all the entries in the matrix (aka grand sum)
 *
 * Dependency: "vector.h"
 */
static double matrix_grand_sum(matrix_t* m)
{
    assert(m != NULL);
    double grand_sum = 0.0;
    int i;
    for(i=0; i<m->num_rows; i++){
        grand_sum += m->matrix[i]->sum(m->matrix[i]);
    }
    return grand_sum;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_pow
 *
 * Arguments: matrix
 *            exponent
 *             (must be a square matrix)
 *
 * Returns: a pointer to a matrix that is the nth power of the inputted matrix

 *
 * Dependency: matrix_is_square
 *             clone_matrix
 *             matrix_multiply
 *             matrix_destroy
 */
static matrix_t* matrix_pow(matrix_t* m, int exponent)
{
    assert(m != NULL);
    assert(exponent > 0 && "Exponent must be greater than 0");
    assert(m->is_square(m) && "Can only take powers of square matrices");
    matrix_t* ret = m->copy(m);
    int i;
    for(i=1; i<exponent; i++){
        matrix_t* temp = ret;
        ret = matrix_multiply(ret, m);
        temp->free(temp);
    }
    return ret;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_col_vec_is_zero
 *
 * Arguments: matrix
 *            column number
 *
 * Returns: 1 if column vector is the zero vector, 0 otherwise
 */
int matrix_col_vec_is_zero(matrix_t* m, int col_num)
{
    assert(m != NULL);
    assert(m->num_columns > col_num && "Column Number too large");
    int i;
    for(i=0; i<m->num_rows; i++){
        if (m->matrix[i]->vector[col_num] != 0)
            return 0;
    }
    return 1;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_eliminate_column
 *
 * Arguments: matrix
 *            row pivot is located on
 *            column to eliminate
 *            pointer to number of rows (swapped in gaussian elimination)
 *            pointer to product of scalars used to multiply rows
 *
 * Returns: void
 *           updates pointers to be used in calculating determinant
 *
 * Dependency: matrix_row_swap
 */
void matrix_eliminate_column(matrix_t* m, int row_pivot, int col_num,
                             int* num_row_swaps,
                             double* scalar_multiple_det)
{
    assert(m->num_columns > col_num);
    int index_highest_pivot, i, j;
    double largest = 0.0;   // The pivot element in divisor stored here

    /* Partial Pivot Method: Swap largest prospective pivot to pivot row */
    for(i=row_pivot; i<m->num_rows; i++){
        double pivot = m->matrix[i]->vector[col_num];
        if (fabs(pivot) > fabs(largest)){
            largest = pivot;
            index_highest_pivot = i;
        }
    }
    if (row_pivot == index_highest_pivot){
        ; // No need to swap with self
    }
    else if (largest != 0.0){
        m->row_swap(m, row_pivot, index_highest_pivot);
        scalar_swap(&m->index_int[row_pivot],
                    &m->index_int[index_highest_pivot],
                    sizeof(int));
        scalar_swap(m->index_str[row_pivot],
                    m->index_str[index_highest_pivot],
                    sizeof(char*));
        *num_row_swaps += 1;
    }
    /* Column vector is zero, so no need to eliminate */
    else if (largest == 0.0){
        return;
    }

    /* For each row below the pivot row */
    for(i=row_pivot+1; i<m->num_rows; i++){
        double lambda = m->matrix[i]->vector[col_num]/m->matrix[row_pivot]->vector[row_pivot];
        if (fabs(lambda) < GAUSS_ELIM_ACCURACY){
            continue;
        }

        /* Eliminate */
        for(j=col_num; j<m->num_columns; j++){
            double v = m->matrix[row_pivot]->vector[j];
            //printf("Old Entry: %lf ==> %lf - (%lf * %lf) = %lf\n", m->matrix[i]->vector[j], m->matrix[i]->vector[j], lambda, v,  m->matrix[i]->vector[j] - (lambda*v));
            if (j==col_num){
                m->matrix[i]->vector[j] = 0.0;
            }
            else{
                //printf("Old: %lf\n", m->matrix[i]->vector[j]);
                m->matrix[i]->vector[j] -= (lambda*v);
                //printf("New (%lf * %lf): %lf\n", lambda, v, m->matrix[i]->vector[j]);
            }

        }
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: gaussian_elimination
 *
 * Arguments: matrix
 *
 * Returns: the determinant of the original matrix
 *
 * Dependency: matrix_row_swap
 *             matrix_eliminate_column
 */
static int gaussian_elimination(matrix_t* m)
{
    int i;
    int row_swaps = 0;
    double scalar_multiple_det = 1.0;

    for(i=0; i<m->num_columns; i++){
        matrix_eliminate_column(m, i, i, &row_swaps, &scalar_multiple_det);
    }
    /* Determinant not defined for non-square matrix */
    if (!m->is_square(m)){
        return 0.0;
    }
    double diagonal = 1.0;
    for(i=0; i<m->num_rows; i++){
        diagonal *= m->matrix[i]->vector[i];
    }
    double det = diagonal;

    /* Swapping rows changes the sign of the determinant */
    if ((row_swaps+1) %2 == 0){
        return -det;
    }
    return det;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_determinant
 *
 * Arguments: matrix
 *
 * Returns: the determinant of the matrix
 *
 * Dependency: matrix_is_square
 *             clone_matrix
 *             gaussian_elimination
 *             destroy_matrix
 */
static double matrix_determinant(matrix_t* m)
{
    assert(m->is_square(m) && "Determinant only defined for square matrices");
    matrix_t* temp = m->copy(m);
    double det = temp->gaussian_elimination(temp);
    temp->free(temp);
    return det;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: matrix_rank
 *
 * Arguments: matrix
 *
 * Returns: the rank of the matrix
 *
 * Dependency: clone_matrix
 *             gaussian_elimination
 *             "vector.h"
 */
static int matrix_rank(matrix_t* m)
{
    matrix_t* clone = m->copy(m);
    clone->gaussian_elimination(clone);
    int i;
    int rank = 0;
    for(i=0; i<clone->num_rows; i++){
        rank += (!clone->matrix[i]->is_zero_vector(clone->matrix[i]));
        if (rank == clone->num_rows || rank == clone->num_columns){
            return rank;
        }
    }
    return rank;
}
//-----------------------------------------------------------------------------

static double matrix_column_mean(matrix_t* m, int col_num)
{
    double sum = 0.0;
    int i;
    for(i=0; i<m->num_columns; i++){
        sum += m->get_entry(m, i, col_num);
    }
    return sum/m->num_columns;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: csv_to_matrix
 *
 * Arguments: csv file name
 *
 * Returns: the rank of the matrix
 *
 * Dependency: clone_matrix
 *             gaussian_elimination
 *             "vector.h"
 */

static void matrix_to_csv(matrix_t* m, char* fname)
{
    FILE *fp = fopen(fname, "w");
    int i, j;
    char empty[] = {"Nan"};
    for(i=0; i<m->num_rows; i++){
        for(j=0; j<m->num_columns; j++){
            double entry = m->get_entry(m, i, j);
            if (entry == DBL_EPSILON){
                fprintf(fp, "%s,", empty);
            }
            else{
                fprintf(fp, "%lf", entry);
                if (j != m->num_columns -1){
                    fprintf(fp, ",");
                }
            }
            
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void print_column_names(matrix_t* m)
{
    assert(m->column_index_used);
    int i;
    for(i=0; i<m->num_columns; i++){
        printf("%s\n", m->column_names[i]);
    }
}

double get_matrix_entry_by_colname(matrix_t* m, int row, char* col_name)
{
    if (!m->column_index_used){
        fprintf(stderr, "You do not have column names associated with the "
                "matrix\n");
        assert(0 && "No column names");
    }
    int index = lsearch(m->column_names, m->num_columns, col_name);
    if (index == -1){
        fprintf(stderr, "No column name exists\n");
        assert(0);
    }
    return m->get_entry(m, row, index);

}

matrix_t* csv_to_matrix(char* fname, char* delim, int num_rows, char* miss_val,
                        int columns_labelled, int rows_labelled)
{
    int i;
    (columns_labelled) ? (i=1) : (i=0);
    int initial_i = i;
    char** buff = malloc((num_rows-i)*sizeof(*buff));
    assert(unwanted_null(buff));
    int lines = read_file(fname, buff, num_rows);
    matrix_t* m = create_matrix(lines-i,12);
    int num_columns = 0;


    for(i=0; i<lines; i++){
        int j = i-initial_i;
        int entry = 0;
        char* token;
        token = strtok(buff[i], ",");
        if (columns_labelled && i==0){
            int k = 0;
            token = strtok(NULL, delim);
            while (token != NULL){
                free(m->column_names[k]);
                m->column_names[k] = malloc((strlen(token)+1)*sizeof(*m->column_names[k]));
                assert(unwanted_null(m->column_names[k]));
                strcpy(m->column_names[k++], token);
                token = strtok(NULL, delim);
            }
            m->column_index_used = 1;
            num_columns = k-1;
            continue;
        }
        else if (rows_labelled){
            free(m->index_str[j]);
            m->index_str[j] = malloc((strlen(token)+1)*sizeof(*m->index_str[j]));
            assert(unwanted_null(m->index_str[j]));
            strcpy(m->index_str[j], token);
            token = strtok(NULL, delim);
        }

        while (token != NULL){
            if (miss_val != NULL && !strcmp(miss_val, token)){
                m->matrix[j]->set(m->matrix[j], entry++, DBL_EPSILON);
            }
            else{
                m->matrix[j]->set(m->matrix[j], entry++, strtod(token, NULL));
            }
            token = strtok(NULL, delim);
        }
        if (i==initial_i){
            num_columns = entry;
        }
        else{
            assert(num_columns == entry);
        }
        m->matrix[j]->dimension = num_columns;
    }
    m->num_columns = num_columns;
    if (rows_labelled){
        m->str_index_used = 1;
    }
    return m;
}

static void matrix_impute_missing_values(matrix_t* m, int mode){
    int i;
    for(i=0; i<m->num_rows; i++){
        m->matrix[i]->impute_missing_value(m->matrix[i], DBL_EPSILON, mode);
    }
}

matrix_t* matrix_to_corrcoef(matrix_t* m, int mode)
{
    assert(m->is_square(m));
    matrix_t* ret = m->copy(m);
    int i, j;
    for(i=0; i<m->num_rows; i++){
        for(j=i+1; j<m->num_rows; j++){
            double entry = vector_correlation(m->matrix[i], m->matrix[j], mode);
            ret->set_entry(ret, i, j, entry);
            ret->set_entry(ret, j, i, entry);
            printf("Covariance: %d %d, %lf\n", i, j, vector_covariance(m->matrix[i], m->matrix[j], mode));
        }
    }
    for(i=0; i<m->num_rows; i++){
        ret->set_entry(ret, i, i, 1.0);
    }
    return ret;
} 
