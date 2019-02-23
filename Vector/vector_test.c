#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>   // for time_t
#include <unistd.h> // for getpid
#include "vector.h"
#include "../Math_Extended/math_extended.h"

#define MAX_DIMENSION 1000

int main(void)
{
    time_t now = time(NULL);
    srand(now ^ (getpid() <<16));

    int dimension = rand();
    int i;

    /* Test creating a zero vector */
    vector_t* v = create_zero_vector(dimension);
    for(i=0; i<dimension; i++){
        if (v->vector[i] != 0){
            printf("create_zero_vector failed, entry: %lf\n", v->vector[i]);
            exit(EXIT_FAILURE);
        }
    }
    v->free(v);
    printf("create_zero_vector Success\n");

    /* Test creating a vector from array */
    double A[] = {1, 1};
    vector_t* v1 = create_vector_from_array(A, 2);
    vector_t* v2 = create_vector_from_array(A, 2);

    if (v1->vector[0] != 1 || v1->vector[1] != 1){
        printf("create_vector_from_array Failure\n");
        exit(EXIT_FAILURE);
    }
    printf("create_vector_from_array Success\n");

    /* Test vector addition */
    vector_t* v3 = vector_addition(v1, v2);
    if (v3->vector[0] != 2 || v3->vector[1] != 2){
        printf("vector_addition Failure\n");
        exit(EXIT_FAILURE);
    }
    printf("vector_addition Success\n");

    /* Test normalising a vector in place */
    v2->normalise(v2);
    if (v2->vector[0] != 1/sqrt(2) || v2->vector[1] != 1/sqrt(2)){
        printf("vector_normalise Failure\n");
        exit(EXIT_FAILURE);
    }
    printf("vector_normalise Success\n");

    /* Test finding the norm of a vector */
    double B[] = {1, 2, 0};
    vector_t* v4 = create_vector_from_array(B, 3);
    if (v4->norm(v4) != sqrt(5)){
        printf("vector_norm Failure\n");
        exit(EXIT_FAILURE);
    }
    printf("vector_norm Success\n");

    v4->set(v4, 3, 10);
    if (v4->vector[3] != 10){
        printf("vector_set Failure\n");
        exit(EXIT_FAILURE);
    }
    printf("vector_set Success\n");
    v4->set(v4, 4, 11);
    v4->set(v4, 5, 12);
    v4->set(v4, 6, 13);
    v4->set(v4, 7, 14);
    v4->set(v4, 8, 15);
    v4->set(v4, 9, 16);
    int resize_to = 5;
    v4->resize(v4, resize_to);
    if(v4->dimension != resize_to){
        printf("vector_resize Failure\n");
        exit(EXIT_FAILURE);
    }
    printf("vector_resize Success\n");

    (vectors_same_dimension(v4, v4) == 1) ?  printf("vectors_same_dimension Success\n"):
                                             printf("vectors_same_dimension Failure\n");

    (vector_equality(v4, v4) == 1) ? printf("vector_equality Success\n"):
                                     printf("vector_equality Failure\n");

    double sum = v4->sum(v4);
    v4->impute_missing_value(v4, 0.0, MEAN);
    if (fabs(v4->vector[2] - sum/(resize_to-1)) > DBL_EPSILON*10){
        printf("vector_impute_missing_value Failure\n");
        exit(EXIT_FAILURE);
    }
    printf("vector_impute_missing_value Success\n");

    (vector_equality(v4, v4) == 1) ? printf("vector_equality Success\n"):
                                     printf("vector_equality Failure\n");


    if (errno == 0){
        printf("All tests successful\n");
    }
    else{
        printf("Error raised: %s\n", strerror(errno));
    }

}