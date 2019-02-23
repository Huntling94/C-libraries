#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include "..\Vector\vector.h"
#include "..\Matrix\matrix.h"
#include "..\Math_Extended\math_extended.h"
#include "..\Files\file.h"



int main (void)
{
    matrix_t* m = csv_to_matrix("test.txt", ",", 4, "-1");
    m->print(m);
}