#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#define MEAN 0
#define MEDIAN 1


#define SAMPLE 0
#define POPULATION 1

typedef struct vector vector_t;

struct vector{
    double* vector;
    int dimension;
    int alloc;

    void (*set)(vector_t* v, int index, double val);
    void (*resize)(vector_t* v, int new_alloc_size);
    void (*print)(vector_t* v);
    double (*sum)(vector_t* v);
    double (*norm)(vector_t* v);
    void (*normalise)(vector_t* v);
    vector_t* (*copy)(vector_t* v);
    void (*free)(vector_t* v);
    int (*is_zero_vector)(vector_t* v);
    double (*arithmetic_mean)(vector_t* v);
    double (*geometric_mean)(vector_t* v);
    double (*standard_deviation)(vector_t* v, int mode);
    void (*impute_missing_value)(vector_t* v, double miss_val, int mode);
};


vector_t* create_zero_vector(int dim);
vector_t* create_vector_from_array(double* src, int n);

double vector_euclidean_distance(vector_t* v1, vector_t* v2);
double vector_manhattan_distance(vector_t* v1, vector_t* v2);
double vector_minkowski_distance(vector_t* v1, vector_t* v2);
double cosine_similarity(vector_t* v1, vector_t* v2);

double vector_dot_product(vector_t* v1, vector_t* v2);
double vector_covariance(vector_t* v1, vector_t* v2, int mode);
double vector_correlation(vector_t* v1, vector_t* v2, int mode);
vector_t* vector_addition(vector_t* v1, vector_t* v2);
vector_t* vector_scalar_multiplication(vector_t* v1, double scalar);
int vectors_same_dimension(vector_t* v1, vector_t* v2);
int vector_equality(vector_t* v1, vector_t* v2);
vector_t* vector_hadamard_product(vector_t* v1, vector_t* v2);

#endif // VECTOR_H
