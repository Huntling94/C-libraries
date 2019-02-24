#include "matrix.h"
#include "..\Files\files.h"
#include "..\Utilities\utils.h"
#include "..\Hashtable\hashtable.h"
#include "..\Math_Extended\math_extended.h"

#define IRIS_DATASET "..\\Test_Data\\Iris.csv"
#define SUCCESS_FAIL (printf("Success\n")) : (printf("Failure\n"))

int main(void){
    double A[] = {3, 4, 5};

    int init_row = 1;
    int init_col = sizeof(A) / sizeof(A[0]);
    printf("Testing create_matrix: ");
    matrix_t* m = create_matrix(init_row, init_col);
    (m->num_rows == init_row
     && m->num_columns == init_col
     && m->column_index_used == 0
     && m->str_index_used == 0) ? SUCCESS_FAIL;

    printf("Testing matrix_set_row: ");
    int success;
    int i;
    m->set_matrix_row(m, A, init_col, 0);
    for(i=0; i<m->num_columns; i++){
        if (m->get_entry(m, 0, i) != A[i]){
            success = 0;
        }
        else{
            success = 1;
        }
    }
    (success) ? SUCCESS_FAIL;

    printf("Testing matrix_grand_sum: ");
    double sum = 0.0;
    for(i=0; i<init_col; i++){
        sum += A[i]; 
    }
    double* s = doub(sum);
    double* gs = doub(m->grand_sum(m));
    (!double_cmp(s, gs)) ? SUCCESS_FAIL;
    free(s); free(gs);

    printf("Testing destroy_matrix: ");
    m->free(m); printf("Success\n");
    
    printf("Testing matrix_is_square: ");
    m = create_matrix(init_col, init_col);
    for(i=0; i<init_col; i++){
        m->set_matrix_row(m, A, init_col, i);
    }
    (m->is_square) ? SUCCESS_FAIL;

    printf("Testing matrix_rank: ");
    if (m->rank(m) == 1){
        matrix_t* temp = csv_to_matrix("rank_test1.csv", ",",
                         lines_in_file("rank_test1.csv"), NULL, 0, 0);
        (temp->rank(temp) == 3) ? SUCCESS_FAIL;
        temp->free(temp);
    }
    else{
        (0) ? SUCCESS_FAIL;
    }


    
    /* matrix_t* iris_matrix = csv_to_matrix(IRIS_DATASET, ",", lines_in_file(IRIS_DATASET), NULL, 1, 0); */



    
    
}