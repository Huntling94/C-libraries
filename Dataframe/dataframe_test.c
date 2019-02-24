#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "dataframe.h"
#include "..\Math_Extended\math_extended.h"
#include "..\Utilities\utils.h"
#include "..\Files\files.h"
#include "..\Hashtable\hashtable.h"

#define IRIS_DATASET "..\\Test_Data\\Iris.csv"

int main(void)
{
    int iris_datatypes[] = {INT, DOUBLE, DOUBLE, DOUBLE, DOUBLE, STRING};
    dataframe_t* iris = csv_to_dataframe(IRIS_DATASET, ",", iris_datatypes, LABELLED);
    iris->head(iris, 10);
    printf("Mean of Column 0: %lf\n", iris->mean(iris, COLUMN, "Id"));
}