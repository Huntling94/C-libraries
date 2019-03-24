#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#include <math.h>
#include "math_extended.h"

int main(void)
{
    printf("%d\n", arg_max_int(5, 20, "Dog", 19, 24, -1));
}