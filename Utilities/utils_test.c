#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "utils.h"

#define SUCCESS_FAIL (printf("Success\n")) : (printf("Failed\n"))

int main(int argc, char* argv[])
{
    int pid = getpid();
    int* pid_copy = integer(pid);
    double* pid_copy_doub = doub((double)pid);
    char* pid_copy_char = character(pid%127);
    printf("Testing integer: "); (pid == *pid_copy) ? SUCCESS_FAIL;
    printf("Testing doub: "); ((double)pid == *pid_copy_doub) ? SUCCESS_FAIL;
    printf("Testing character: "); ((char)(pid%127) == pid_copy_char[0]) ? SUCCESS_FAIL;

    char* arg = copy_string(argv[0]);
    printf("Testing copy_string: "); (strcmp(arg, argv[0]) == 0) ? SUCCESS_FAIL;


    *pid_copy = pid/3;
    if (pid != *pid_copy){
        printf("Testing scalar_swap: ");
        int old_pid = pid;
        int old_pid_copy = *pid_copy;
        scalar_swap(&pid, pid_copy, sizeof(int));
        
        if (pid == old_pid_copy && *pid_copy == old_pid){
            scalar_swap(&pid, pid_copy, sizeof(int));
            (pid == old_pid && *pid_copy == old_pid_copy) ? SUCCESS_FAIL;
        }
    }

    free(pid_copy);
    free(pid_copy_doub);
    free(pid_copy_char);
    free(arg);
}