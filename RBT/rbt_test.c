#include <windows.h>
#include <time.h>
#include <unistd.h>
#include "rbt.h"

#define MAX_TESTING_ELEMENTS 10000
#define MAX_STRING_LENGTH 50
#define SEPARATOR printf("-\n-\n-\n")

int main(void)
{
    time_t now = time(NULL);
    srand(now ^ (getpid() <<16));

    rbt_t* t = create_rbt(int_cmp);
    printf("Tree successfully created\n");

    int i, j;
    int length = rand()%MAX_TESTING_ELEMENTS;

    printf("Testing RBT of Integers\n");
    for(i=0; i<length; i++){
        t->insert(t, integer(rand()));
    }

    t->print(t, print_int);
    printf("Size of tree: %d\n", t->len(t));
    if(t->len(t) > length){
        fprintf(stderr, "Failure size of tree larger than num elements inserted\n");
        exit(EXIT_FAILURE);
    }
    t->destroy(t, free);

    printf("The above printing should print the integers in order. If it does "
           "then RBT of Integers success.\n");

    SEPARATOR;

    printf("Testing RBT of Doubles\n");
    t = create_rbt(double_cmp);
    length = rand()%MAX_TESTING_ELEMENTS+1;
    for(i=0; i<length; i++){
        t->insert(t, doub(rand()*1.0/(i+1)));
    }
    t->insert(t, doub(0.001));
    t->insert(t, doub(0.0001));
    t->insert(t, doub(0.00001));
    t->insert(t, doub(0.000001));
    t->print(t, print_double);
    printf("Size of tree: %d\n", t->len(t));
    if(t->len(t) > length+4){
        fprintf(stderr, "Failure size of tree larger than num elements inserted\n");
        exit(EXIT_FAILURE);
    }
    t->destroy(t, free);

    printf("The above printing should print the doubles in order. If it does "
           "then RBT of Doubles success.\n");

    SEPARATOR;

    printf("Testing RBT of Strings\n");
    create_rbt(str_cmp);
    int num_strings = rand()%MAX_TESTING_ELEMENTS;
    for(i=0; i<num_strings; i++){
        length = rand()%(MAX_STRING_LENGTH+1);
        char* test = malloc(sizeof(*test) * (length + 1));
        for(j=0; j<length; j++){
            int ascii = rand()%26 + 96;
            test[j] = (char)ascii;
        }
        test[j] = '\0';
        t->insert(t, test);
    }

    t->print(t, print_str);
    printf("Size of tree: %d\n", t->len(t));
    if(t->len(t) > num_strings){
        fprintf(stderr, "Failure size of tree larger than num elements inserted\n");
        exit(EXIT_FAILURE);
    }
    t->destroy(t, free);

    printf("The above printing should print the strings in order. If it does "
           "then RBT of Strings success.\n");

    SEPARATOR;

    printf("Testing RBT of Chars\n");
    t = create_rbt(char_cmp);
    length = rand()%MAX_TESTING_ELEMENTS+1;
    for(i=0; i<length; i++){
        t->insert(t, character((char)(rand()%26 + 96)));
    }

    t->print(t, print_char);
    printf("Size of tree: %d\n", t->len(t));
    if(t->len(t) > num_strings){
        fprintf(stderr, "Failure size of tree larger than num elements inserted\n");
        exit(EXIT_FAILURE);
    }
    printf("Frequency of b: %d\n", t->get_frequency(t, "b"));
    printf("Colour of b: "); (t->get_colour == RED) ? (printf("Red\n")) : (printf("Black\n"));
    t->destroy(t, free);

    printf("The above printing should print the strings in order. If it does "
           "then RBT of Chars success.\n");

    SEPARATOR;

    printf("Tests successful\n");
    exit(EXIT_SUCCESS);
}