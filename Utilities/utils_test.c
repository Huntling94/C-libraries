#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "utils.h"

#define SUCCESS_FAIL (printf("Success\n")) : (printf("Failed\n"))

int main(void)
{
    int pid = getpid();
    int* pid_copy = integer(pid);
    double* pid_copy_doub = doub((double)pid);
    char* pid_copy_char = character(pid%127);
    printf("Testing integer: "); (pid == *pid_copy) ? SUCCESS_FAIL;
    printf("Testing doub: "); ((double)pid == *pid_copy_doub) ? SUCCESS_FAIL;
    printf("Testing character: "); ((char)(pid%127) == pid_copy_char[0]) ? SUCCESS_FAIL;
}