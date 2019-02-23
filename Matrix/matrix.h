#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "..\Vector\vector.h"


#ifndef MATRIX_H
#define MATRIX_H

#define GAUSS_ELIM_ACCURACY 1e-26
#define LABELLED 1
#define NOT_LABELLED 0

typedef struct matrix matrix_t;

struct matrix{
    vector_t** matrix;
    int* index_int;
    char** index_str;
    char** column_names;
    int column_index_used;
    int str_index_used;
    int num_rows;
    int alloc_rows;
    int num_columns;
    int alloc_columns;

    void (*print)(matrix_t* m);
    void (*print_head)(matrix_t* m, int rows);
    double (*get_entry)(matrix_t* m, int row, int col);
    void (*set_entry)(matrix_t* m, int row, int col, double entry);
    void (*set_matrix_row)(matrix_t* m, double* src, int dim, int row_num);
    matrix_t* (*copy)(matrix_t* m);
    void (*free)(matrix_t* m);
    void (*row_swap)(matrix_t*m, int, int);
    matrix_t* (*transpose)(matrix_t* m);
    double (*trace)(matrix_t* m);
    int (*rank)(matrix_t* m);
    double (*determinant)(matrix_t* m);
    double (*grand_sum)(matrix_t* m);
    double (*matrix_column_mean)(matrix_t* m, int col_num);
    int(*is_square)(matrix_t* m);
    matrix_t* (*pow)(matrix_t* m, int e);
    int (*gaussian_elimination)(matrix_t* m);

    void(*impute_missing_values)(matrix_t* m, int mode);
    void(*to_csv)(matrix_t* m, char* fname);
};

matrix_t* create_matrix(int rows, int columns);
matrix_t* csv_to_matrix(char* fname, char* delim, int num_rows, char* miss_val,
                        int columns_labelled, int rows_labelled);
void print_index(matrix_t* m);
matrix_t* matrix_to_corrcoef(matrix_t* m, int mode);

matrix_t* matrix_addition(matrix_t* m1, matrix_t* m2);
matrix_t* matrix_multiply(matrix_t* m1, matrix_t* m2);
matrix_t* matrix_hadamard_product(matrix_t* m1, matrix_t* m2);

void print_column_names(matrix_t* m);
double get_matrix_entry_by_colname(matrix_t* m, int row, char* col_name);

#endif // MATRIX_H
