#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#include <math.h>
#include "math_extended.h"

#define INT 0
#define STRING 1
#define DOUBLE 2
#define CHAR 3
#define FLOAT 4
#define LONG 5

int arg_max_int(int count, ...)
{
    va_list ap;
    int max = INT_MIN;
    int val, i;
    va_start(ap, count);
    for(i=0; i<count; i++){
        val = va_arg(ap, int);
        if (val > max){
            max = val;
        }
    }
    va_end(ap);
    return max;
    
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: nth_root
 *
 * Arguments: a double value
 *            an integer nth root
 *
 * Returns: returns the nth root.
 */
double nth_root(double x, int n)
{
    double delta, y = 1.0;
    if (n==0){
        return 0.0;
    }
    else if (n==1){
        return x;
    }
    do{

        delta = (x / pow(y, n-1) - y) / n;
        y += delta;
    } while(fabs(delta) >= DBL_EPSILON*10000);
    return y;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: factorial
 *
 * Arguments: factorial of n
 *
 * Returns: the nth factorial
 *
 * Note, if n is greater than 12 the result will integer overflow, so -1 is
 * returned. Likewise, factorials are not defined for negative integers, so
 * -1 is returned also.
 */
int factorial(int n)
{
    if (n == 1 || n == 0){
        return 1;
    }
    else if (n < 0 || n > 12){
        return -1;
    }
    return n * factorial(n-1);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: fibonacci
 *
 * Arguments: fibonacci of n
 *
 * Returns: the nth fibonacci
 */
int fibonacci(int n)
{
    if (n == 0){
        return 0;
    }
    else if (n == 1){
        return 1;
    }
    return fibonacci(n-2) + fibonacci(n-1);
}

double log_loss(int true_label, double predicted_prob)
{
    if (predicted_prob >= 1){
        predicted_prob = 1 - 10e-15;
    }
    else if (predicted_prob <= 0){
        predicted_prob = 0 + 10e-15;
    }
    assert(true_label == 1 || true_label == 0);
    if (true_label == 1){
        return -log(predicted_prob);
    }
    return -log(1 - predicted_prob);
}

double accuracy(int num_correct, int num_matches){
    assert(num_matches >= num_correct);
    if (num_matches == 0){
        return 0.0;
    }
    return num_correct/num_matches;

}