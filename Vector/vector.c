#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "vector.h"
#include "../Math_Extended/math_extended.h"
#include "../Utilities/utils.h"



/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_zero_vector
 *
 * Arguments: uninitialized vector
 *            dimension of the vector (number of components)
 *
 * Returns: pointer to a zero vector that stores doubles
 */
static void vector_add_function_pointers(vector_t* v);
static void vector_set(vector_t* v, int index, double val);
static void print_vector(vector_t* v);
static double vector_sum(vector_t* v);
static double vector_norm(vector_t* v);
static void vector_normalise(vector_t* v);
static double vector_arithmetic_mean(vector_t* v);
static double vector_geometric_mean(vector_t* v);
static void destroy_vector(vector_t* v);
static int vector_is_zero(vector_t* v);
static void vector_impute_missing_value(vector_t* v, double miss_val, int mode);
static double vector_standard_deviation(vector_t* v1, int mode);

vector_t* create_zero_vector(int dim)
{
    vector_t* v = malloc(sizeof(*v));
    assert(unwanted_null(v));
    assert(dim >= 0);
    v->dimension = dim;
    v->alloc = dim;
    v->vector = calloc(sizeof(*v->vector) * v->alloc, sizeof(double));
    assert(unwanted_null(v->vector));

    vector_add_function_pointers(v);
    return v;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: print_vector
 *
 * Arguments: the vector
 *
 * Returns: void
 */
static void print_vector(vector_t* v)
{
    int i;
    double zero = 0.0;
    for(i=0; i<v->dimension; i++){
        /* This check stops printing -0.0 */
        if (v->vector[i] == zero){
            v->vector[i] = zero;
        }
        if (v->vector[i] == DBL_EPSILON){
            printf("  Nan ");
        }
        else{
            printf("%10.1lf ", v->vector[i]);
        }
    }
    printf("\n");
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: create_vector_from_array
 *
 * Arguments: uninitialized vector
 *            array of doubles
 *            number of elements in array
 *
 * Returns: pointer to a vector with components equal to the array
 */
vector_t* create_vector_from_array(double* src, int n)
{
    vector_t* v = malloc(sizeof(*v));
    assert(unwanted_null(v));
    v->dimension = n;
    v->alloc = n;
    v->vector = malloc(sizeof(*v->vector) * v->alloc);
    assert(unwanted_null(v->vector));
    int i;
    for(i=0; i<n; i++){
        v->vector[i] = src[i];
    }
    vector_add_function_pointers(v);
    return v;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: clone_vector
 *
 * Arguments: vector you want to create a copy of
 *
 * Returns: pointer to a vector with components equal to the source vectors
 */
static vector_t* clone_vector(vector_t* src)
{
    assert(src != NULL);
    vector_t* dest;
    dest = malloc(sizeof(*dest));
    assert(unwanted_null(dest));
    dest->alloc = dest->dimension = src->dimension;
    dest->vector = malloc(sizeof(*dest->vector) * dest->alloc);
    assert(unwanted_null(dest->vector));
    int i;
    for(i=0; i<src->dimension; i++){
        dest->vector[i] = src->vector[i];
    }
    vector_add_function_pointers(dest);
    return dest;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_set
 *
 * Arguments: vector you want to modify
 *            index of vector you want to set (counting from 0)
 *            value you want to set that slot to
 *
 * Returns: void
 *           Note: Dynamically resizes vector as required
 */
static void vector_set(vector_t* v, int index, double val)
{
    assert(v != NULL);
    if (index > v->dimension){
        assert(0 && "Vector dimension too small");
    }
    else if (index >= v->alloc){
        v->alloc *= 2;
        v->vector = realloc(v->vector, v->alloc*sizeof(*v->vector));
        assert(unwanted_null(v->vector));
    }
    v->vector[index] = val;
    if (index == v->dimension){
        v->dimension++;
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_resize
 *
 * Arguments: vector you want to resize
 *            new maximum allocated size of vector
 *
 * Returns: void
 *           Note: Will dynamically add more space to vector if asked for
 *           Note: Will trim size of vector (and reduce dimension) if new size
 *                 is less than old size
 */
static void vector_resize(vector_t* v, int new_alloc_size){
    if (new_alloc_size < v->alloc){
        v->dimension = new_alloc_size;
    }
    else if (new_alloc_size == v->alloc){
        return;
    }
    v->alloc = new_alloc_size;
    v->vector = realloc(v->vector, v->alloc*sizeof(*v->vector));
    assert(unwanted_null(v->vector));
}
//-----------------------------------------------------------------------------

static void vector_add_function_pointers(vector_t* v)
{
    assert(v != NULL);
    v->set = &vector_set;
    v->resize = &vector_resize;
    v->print = &print_vector;
    v->sum = &vector_sum;
    v->norm = &vector_norm;
    v->normalise = &vector_normalise;
    v->copy = &clone_vector;
    v->free = &destroy_vector;
    v->is_zero_vector = &vector_is_zero;
    v->arithmetic_mean = &vector_arithmetic_mean;
    v->geometric_mean = &vector_geometric_mean;
    v->standard_deviation = &vector_standard_deviation;
    v->impute_missing_value = &vector_impute_missing_value;
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_dot_product
 *
 * Arguments: vector 1
 *            vector 2
 *
 * Returns: dot product between the two vectors
 *
 * Dependency: vector_hadamard_product
 *             vector_sum
 *             destroy_vector
 */
double vector_dot_product(vector_t* v1, vector_t* v2)
{
    assert(v1 != NULL && v2 != NULL);
    vector_t* temp = vector_hadamard_product(v1, v2);
    double dot_product = temp->sum(temp);
    temp->free(temp);
    return dot_product;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_addition
 *
 * Arguments: vector 1
 *            vector 2
 *             (must be same size)
 *
 * Returns: A third vector whose components are the sum of the components
 *          of the first two vectors
 *
 * Dependency: clone_vector
 */
vector_t* vector_addition(vector_t* v1, vector_t* v2)
{
    assert(v1 != NULL && v2 != NULL);
    if (v1->dimension != v2->dimension){
        assert(0 && "Can only add vectors of same dimension");
    }
    vector_t* v3 = v1->copy(v1);
    int i;
    for(i=0; i<v2->dimension; i++){
        v3->vector[i] += v2->vector[i];
    }
    return v3;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_scalar_multiplication
 *
 * Arguments: vector 1
 *            a scalar to multiply each vector component by
 *
 * Returns: A second vector whose components are a scalar multiple of the input
 *
 * Dependency: clone_vector
 */
vector_t* vector_scalar_multiplication(vector_t* v1, double scalar)
{
    assert(v1 != NULL);
    vector_t* v2 = v1->copy(v1);
    int i;
    for(i=0; i<v2->dimension; i++){
        v2->vector[i] *= scalar;
    }
    return v2;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_hadamard_product
 *
 * Arguments: vector 1
 *            vector 2
 *
 * Returns: A third vector whose components are the product of the components
 *          of the first two vectors.
 *           Note that a vector is equivalent to a 1 x Dim(v) matrix.
 *
 * Dependency: clone_vector
 */
vector_t* vector_hadamard_product(vector_t* v1, vector_t* v2)
{
    assert(v1 != NULL && v2 != NULL);
    if (v1->dimension != v2->dimension){
        assert(0 && "Vectors not same dimension");
    }
    vector_t* v3 = v1->copy(v1);
    int i;
    for(i=0; i<v2->dimension; i++){
        v3->vector[i] *= v2->vector[i];
    }
    return v3;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_sum
 *
 * Arguments: a vector
 *
 * Returns: The sum of all the components in the vector
 */
static double vector_sum(vector_t* v)
{
    double ret = 0.0;
    int i;
    for(i=0; i<v->dimension; i++){
        ret += v->vector[i];
    }
    return ret;
}
//-----------------------------------------------------------------------------


/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: destroy_vector
 *
 * Arguments: a vector
 *
 * Returns: void
 */
static void destroy_vector(vector_t* v)
{
    assert(v != NULL);
    free(v->vector);
    free(v);
    v = NULL;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_norm
 *
 * Arguments: a vector
 *
 * Returns: The norm of the vector
 *
 * Dependency: vector_hadamard_product
 *             vector_sum
 *             destroy_vector
 */
static double vector_norm(vector_t* v)
{
    vector_t* temp = vector_hadamard_product(v, v);
    double norm = sqrt(temp->sum(temp));
    temp->free(temp);
    return (norm);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_is_zero
 *
 * Arguments: a vector
 *
 * Returns: 1 if vector is zero vector, otherwise 0.
 *
 * Dependency: vector_hadamard_product
 *             vector_sum
 *             destroy_vector
 */
static int vector_is_zero(vector_t* v)
{
    int i;
    for(i=0; i<v->dimension; i++){
        if (v->vector[i]){
            return 0;
        }
    }
    return 1;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_normalise
 *
 * Arguments: a vector
 *
 * Returns: void
 *           the function normalises the vector in place
 *
 * Dependency: vector_hadamard_product
 *             vector_sum
 *             destroy_vector
 */
static void vector_normalise(vector_t* v)
{
     /* Zero vector cannot be normalised */
    if(v->is_zero_vector(v)){
        return;
    }
    double norm = v->norm(v);
    int i;
    for(i=0; i<v->dimension; i++){
        v->vector[i] /= norm;
    }
}
//-----------------------------------------------------------------------------

int vectors_same_dimension(vector_t* v1, vector_t* v2)
{
    assert(v1 != NULL && v2 != NULL);
    return (v1->dimension == v2->dimension);
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_euclidean_distance
 *
 * Arguments: vector 1
 *            vector 2
 *
 * Returns: the euclidean distance between the two vectors
 *
 * Dependency: vectors_same_dimension
 *             vector_scalar_multiplication
 *             vector_addition
 *             vector_hadamard_product
 *             vector_sum
 *             vector_destroy
 */
double vector_euclidean_distance(vector_t* v1, vector_t* v2)
{
    assert(vectors_same_dimension(v1, v2));
    vector_t* temp = vector_scalar_multiplication(v1, -1);
    vector_t* temp2 = vector_addition(temp, v2);
    temp->free(temp);
    temp = vector_hadamard_product(temp2, temp2);
    temp2->free(temp2);
    double dist = sqrt(temp->sum(temp));
    temp->free(temp);
    return dist;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_manhattan_distance
 *
 * Arguments: vector 1
 *            vector 2
 *
 * Returns: the manhattan distance between the two vectors
 *           sum of absolute value of distance between components
 *
 * Dependency: vectors_same_dimension
 */
double vector_manhattan_distance(vector_t* v1, vector_t* v2)
{
    assert(vectors_same_dimension(v1, v2));
    double dist = 0.0;
    int i;
    for(i=0; i<v1->dimension; i++){
        dist += fabs(v1->vector[i] - v2->vector[i]);
    }
    return dist;

}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_minkowski_distance
 *
 * Arguments: vector 1
 *            vector 2
 *
 * Returns: the manhattan distance between the two vectors
 *           sum of absolute value of distance between components
 *
 * Dependency: vectors_same_dimension
 *             math_extended.h
 */
double vector_minkowski_distance(vector_t* v1, vector_t* v2)
{
    assert(vectors_same_dimension(v1, v2));
    double dist = 0.0;
    int i;
    for(i=0; i<v1->dimension; i++){
        dist += (pow(fabs(v1->vector[i] - v2->vector[i]), v1->dimension));
    }
    return nth_root(dist, v1->dimension);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: cosine_similarity
 *
 * Arguments: vector 1
 *            vector 2
 *
 * Returns: the cosine of the angle between the two vectors
 *
 * Dependency: vectors_same_dimension
 *             vector_dot_product
 *             vector_norm
 */
double cosine_similarity(vector_t* v1, vector_t* v2)
{
    assert(vectors_same_dimension(v1, v2));
    return vector_dot_product(v1, v2)/(v1->norm(v1) * v2->norm(v2));
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_arithmetic_mean
 *
 * Arguments: vector
 *
 * Returns: arithmetic mean of the components of the vector
 *
 * Dependency: vector_sum
 */
static double vector_arithmetic_mean(vector_t* v)
{
    assert(v != NULL);
    return(v->sum(v)/v->dimension);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_geometric_mean
 *
 * Arguments: vector
 *
 * Returns: geometric mean of the components of the vector
 *
 * Dependency: math_extended.h
 */
static double vector_geometric_mean(vector_t* v)
{
    assert(v != NULL);
    double product = 1.0;
    int i;
    for(i=0; i<v->dimension; i++){
        product *= v->vector[i];
    }
    if(product < 0.0){
        return 0.0;
    }
    return(nth_root(product, v->dimension));
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_equality
 *
 * Arguments: a vector
 *            another vector
 *
 * Returns: 0 if vectors are not equal, otherwise 1
 *
 * Dependency: vectors_same_dimension
 */
int vector_equality(vector_t* v1, vector_t* v2){
    assert(vectors_same_dimension(v1, v2));
    int i;
    for(i=0; i<v1->dimension; i++){
        if (v1->vector[i] != v2->vector[i]){
            return 0;
        }
    }
    return 1;

}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_impute_missing_value
 *
 * Arguments: vector
 *            value that denotes missing value
 *            method of imputation
 *
 * Returns: None. By side effect, imputes missing values of vector
 *
 * Dependency: None
 */
static void vector_impute_missing_value(vector_t* v, double miss_val, int mode)
{
    assert(v != NULL);
    if (v->dimension == 0){
        return;
    }
    switch(mode){
        case MEAN:;
            double sum = 0.0;
            int i;
            int count = 0;
            for(i=0; i<v->dimension; i++){
                if (v->vector[i] != miss_val){
                    count++;
                    sum += v->vector[i];
                }
            }
            double mean = 0.0;
            if (count != 0){
                mean = sum/count;
            }
            for(i=0; i<v->dimension; i++){
                if (v->vector[i] == miss_val){
                    v->vector[i] = mean;
                }
            }
            break;
        default:
            return;

    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_standard_deviation
 *
 * Arguments: vector
 *            mode: SAMPLE or POPULATION
 *
 * Returns: standard deviation of vector
 *
 * Dependency: vector_arithmetic_mean
 */
static double vector_standard_deviation(vector_t* v1, int mode)
{
    if (mode == SAMPLE && v1->dimension == 1){
        return 0.0;
    }
    double mean = v1->arithmetic_mean(v1);
    double sum = 0.0;
    int i;
    for(i=0; i<v1->dimension; i++){
        double difference = v1->vector[i] - mean;
        sum += (difference * difference);
    }
    switch(mode){
        case SAMPLE: return sqrt((sum/(v1->dimension-1)));
        case POPULATION: return sqrt(sum/v1->dimension);
        default: printf("Mode not recognised\n"); assert(0);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_covariance
 *
 * Arguments: a vector
 *            another vector
 *            mode of calculation: SAMPLE or POPULATION
 *
 * Returns: covariance of vector
 *
 * Dependency: vectors_same_dimension
 *             vector_arithmetic_mean
 */
double vector_covariance(vector_t* v1, vector_t* v2, int mode)
{
    assert(vectors_same_dimension(v1, v2));
    if (v1->dimension == 1){
        return 0.0;
    }
    double v1_mean = v1->arithmetic_mean(v1);
    double v2_mean = v2->arithmetic_mean(v2);
    double sum = 0.0;
    int i;
    for(i=0; i<v1->dimension; i++){
        double v1_diff = v1->vector[i] - v1_mean;
        double v2_diff = v2->vector[i] - v2_mean;
        sum += v1_diff * v2_diff;
    }
    switch(mode){
        case SAMPLE: return sum/(v1->dimension-1);
        case POPULATION: return sum/v1->dimension;
        default: printf("Mode not recognised\n"); assert(0);
    }

}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: vector_covariance
 *
 * Arguments: a vector
 *            another vector
 *            mode of calculation: SAMPLE or POPULATION
 *
 * Returns: correlation between the two vectors
 *
 * Dependency: vector_equality
 *             vector_arithmetic_mean
 *             vector_scalar_multiplication
 *             vector_covariance
 *             vector_standard_deviation
 */
double vector_correlation(vector_t* v1, vector_t* v2, int mode)
{
    if (vector_equality(v1, v2)){
        return 1.0;
    }
    else{
        vector_t* v3 = vector_scalar_multiplication(v1, -1);
        if (vector_equality(v3, v2)){
            free(v3);
            return -1.0;
        }
        free(v3);
    }
    double cov = vector_covariance(v1, v2, mode);
    double std1 = v1->standard_deviation(v1, mode);
    double std2 = v2->standard_deviation(v2, mode);
    if (std1 == 0.0 || std2 == 0.0){
        return 0.0;
    }
    return cov/(std1 * std2);
}
//-----------------------------------------------------------------------------