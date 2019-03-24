#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "files.h"
#include "../Utilities/utils.h"


/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: lines_in_file
 *
 * Arguments: file name
 *
 * Returns: number of lines in the file
 */
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
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: columns_in_file
 *
 * Arguments: file name
 *            delimiting character
 *
 * Returns: number of colums in the file, delimited by delim
 */
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
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: read_file
 *
 * Arguments: file name
 *            buffer file is to be stored into
 *            number of rows in the file
 *
 * Returns: returns number of lines (rows) in the file.
 *          by side effect, buff now contains file
 */
int read_file(char* fname, char** buff, int num_rows)
{
    FILE *fp = fopen(fname, "r");
    assert(unwanted_null(fp));
	int c;          // Character read in
	int lines=0;    // Lines of file
	int i=0;      // Characters per line
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
            buff[lines] = (char*)realloc(buff[lines],
                           alloc*sizeof(*buff[lines]));
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
	return lines;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: copy_file
 *
 * Arguments: file name of source file
 *            file name of destination file
 *
 * Returns: returns 0 on success, otherwise 1
 *          by side effect, new copied file with dest name should appear in cd
 */
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
    free_string_array(buff, lines);
    if (errno == initial_err){
        return 1;
    }
    return 0;
}
//-----------------------------------------------------------------------------