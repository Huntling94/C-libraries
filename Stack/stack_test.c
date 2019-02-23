#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stack.h"

#define PALINDROME "tattarrattat"

int main(void){
    int desired_sum = 2+3+5+7+9+11+13+17+19+23+29;
    stack_t* stack = create_empty_stack(1);
    stack->push(stack, integer(2));
    stack->push(stack, integer(3));
    stack->push(stack, integer(5));
    stack->push(stack, integer(7));
    stack->push(stack, integer(9));
    stack->push(stack, integer(11));
    stack->push(stack, integer(13));
    stack->push(stack, integer(17));
    stack->push(stack, integer(19));
    stack->push(stack, integer(23));
    stack->push(stack, integer(29));
    int test_sum = 0;
    while (!stack->is_empty(stack)){
        int* temp = stack->pop(stack);
        test_sum += *temp;
        free(temp);
    }
    stack->destroy(stack, free);
    (test_sum == desired_sum) ? printf("Integer Stack Test Successful\n"): 
                                printf("Integer Stack Test Failure\n");

    stack = create_empty_stack(1);
    stack->push(stack, doub(2));
    stack->push(stack, doub(3));
    stack->push(stack, doub(5));
    stack->push(stack, doub(7));
    stack->push(stack, doub(9));
    stack->push(stack, doub(11));
    stack->push(stack, doub(13));
    stack->push(stack, doub(17));
    stack->push(stack, doub(19));
    stack->push(stack, doub(23));
    stack->push(stack, doub(29));
    test_sum = 0;
    while (!stack->is_empty(stack)){
        double* temp = stack->pop(stack);
        test_sum += *temp;
        free(temp);
    }
    stack->destroy(stack, free);
    (test_sum == desired_sum) ? printf("Double Stack Test Successful\n"): 
                                printf("Double Stack Test Failure\n");


    /* TESTING CHARACTER STACK NOW
     * Since a stack can be used to reverse a string, and a palindrome
     * is the same when reversed, we test by comparing our palindrome
     * with the resultant word produced by the stack
     */
    stack = create_empty_stack(1);
    char word[] = {PALINDROME};
    int i;
    for(i=0; i<strlen(word); i++){
        stack->push(stack, character(word[i]));
    }
    char test_word[strlen(word)+1];
    i=0;
    while(!stack->is_empty(stack)){
        char* temp = stack->pop(stack);
        test_word[i++] = *temp;
        free(temp);
    }
    stack->destroy(stack, free);
    test_word[i] = '\0';
    (!strcmp(word, test_word)) ? printf("Character Stack Test Successful\n"):
                                printf("Character Stack Test Failure\n");
    exit(EXIT_SUCCESS);
}