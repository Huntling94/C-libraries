#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "files.h"


#define IRIS_DATASET "../Test_Data/Iris.csv"
#define IRIS_LINES 151
#define IRIS_COLUMNS 6

int main(void)
{
    printf("Testing lines_in_file: ");
    (lines_in_file(IRIS_DATASET) == IRIS_LINES) ? (printf("Success\n")) :
                                                  (printf("Failed\n"));
    
    printf("Testing columns_in_file: ");
    (columns_in_file(IRIS_DATASET, ",") == IRIS_COLUMNS) ? (printf("Success\n")) :
                                                           (printf("Failed\n"));
}