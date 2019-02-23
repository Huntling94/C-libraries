#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "files.h"
#include "..\Utilities\utils.h"

int lines_in_file(char* fname)
{
    FILE *fp = fopen(fname, "r");
    assert(unwanted_null(fp));

    int c, lines=0;

    while((c=getc(fp)) != EOF){
        (c == '\n') ? lines++ : lines;
    }
    fclose(fp);
    return lines;
}

int columns_in_file(char* fname, char* delim)
{
    FILE *fp = fopen(fname, "r");
    assert(unwanted_null(fp));
    int c, columns = 1;
    int initial = 100;
    char* str = (char*)malloc(initial * sizeof(*str));
    assert(unwanted_null(str));
    int len = 0;
    int alloc = initial;

    while ((c=getc(fp)) != EOF){
        if (c == '\n'){
            break;
        }
        if (len == alloc){
            alloc *= 2;
            str = (char*)realloc(str, alloc * (sizeof*str));
            assert(unwanted_null(str));
        }
        str[len++] = c;
    }
    str[len] = '\0';
    int i;
    for(i=0; i<len; i++){
        columns += (str[i] == delim[0]);
    }
    free(str);
    fclose(fp);
    return columns;

}

int read_file(char* fname, char** buff, int num_rows)
{
    FILE *fp = fopen(fname, "r");
    assert(unwanted_null(fp));
	int c;          // Character read in
	int lines=0;    // Lines of file
	int i = 0;      // Characters per line
    unsigned int alloc = 30;
    int alloc_lines = num_rows;
    assert(buff != NULL);
	while ((c = getc(fp)) != EOF) {
        if (i==0){
            buff[lines] = (char*)malloc(alloc*sizeof(*buff[lines]));
            assert(unwanted_null(buff[lines]));
        }
        else if (strlen(buff[lines]) >= alloc){
            alloc *= 2;
            buff[lines] = (char*)realloc(buff[lines], alloc*sizeof(*buff[lines]));
            assert(unwanted_null(buff[lines]));
        }
        buff[lines][i++] = c;
        /* Add null byte to close string for line */
        if (c == '\n') {
            if (lines == alloc_lines){
                assert(0 && "Insufficient lines allocated");
            }
            buff[lines++][i-1] = '\0';
            i = 0;
            alloc = 30;

        }
	}

	fclose(fp);
    //fprintf(stderr, "\tFile read and closed successfully: %d lines read\n", lines);
	return lines;
}

int copy_file(char* src, char* dest)
{
    int initial_err = errno;
    FILE *fp_dest = fopen(dest, "w");
    assert(unwanted_null(fp_dest));
    int lines = lines_in_file(src);
    char** buff = (char**)malloc(lines * sizeof(*buff));
    assert(unwanted_null(buff));
    read_file(src, buff, lines);
    int i;
    for(i=0; i<lines; i++){
        fprintf(fp_dest, "%s\n", buff[i]);
    }
    fclose(fp_dest);
    if (errno == initial_err){
        return 1;
    }
    return 0;

}