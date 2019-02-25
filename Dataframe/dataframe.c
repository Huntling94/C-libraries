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

#define SUCCESSFULLY_SET_SERIES_ENTRY 101
#define FAILURE_COL_NUM_EXCEED_SERIES_LEN 199

#define PRINT_ROW 0
#define PRINT_COLUMN 1



#define NOT_UNIQUE 0
#define UNIQUE 1

#define WR_POSTPEND " WR"

static int  df_size(dataframe_t* df);
static void df_columns(dataframe_t* df);
static void dataframe_dtypes(dataframe_t* df);
static void dataframe_print_head(dataframe_t* df, int n);
static void dataframe_print_tail(dataframe_t* df, int n);
static void dataframe_print_condition(dataframe_t* df, char* col_name, void* condition);
static void df_delete_columns(dataframe_t* df, char** col_names, int n);
static void df_resize(dataframe_t* df, int new_size);
static int  df_nunique_col(dataframe_t* df, char* col_name, int dropna);
static void df_column_rename(dataframe_t* df, char* old_name, char* new_name);
static void df_colswap(dataframe_t* df, int c1, int c2);

static void df_applymerge(dataframe_t* df, char* col_name1, char* col_name2, int mode);
static void df_frequency_table_col(dataframe_t* df, char* col_name);
static double df_mean(dataframe_t* df, int axis, char* name);

dataframe_t* csv_to_dataframe(char* fname, char* delim, int* datatypes, int columns_named)
{
    dataframe_t* ret = malloc(sizeof(*ret));
    assert(unwanted_null(ret));
    int lines = lines_in_file(fname);
    ret->num_rows = lines-columns_named;
    ret->num_columns = columns_in_file(fname, delim);
    int num_columns = ret->num_columns;
    char** buff = malloc(lines * sizeof(*buff));
    assert(unwanted_null(buff));
    read_file(fname, buff, lines);
    ret->df = malloc(lines * sizeof(*ret->df));
    ret->alloc_rows = lines;
    ret->datatypes = malloc(num_columns * sizeof(*ret->datatypes));
    ret->alloc_columns = num_columns;
    ret->formatting = malloc(num_columns * sizeof(*ret->formatting));
    ret->column_names = malloc(num_columns * sizeof(*ret->column_names));
    ret->column_names_exist = columns_named;

    assert(unwanted_null(ret->df));
    assert(unwanted_null(ret->datatypes));
    assert(unwanted_null(ret->formatting));
    assert(unwanted_null(ret->column_names));

    int i;
    for(i=0; i<num_columns; i++){
        ret->datatypes[i] = datatypes[i];
        switch(ret->datatypes[i]){
            case INT: ret->formatting[i] = 1; break;
            case STRING: ret->formatting[i] = 1; break;
            case DOUBLE: ret->formatting[i] = 1; break;
            default: assert(0);
        }

    }
    if (ret->column_names_exist == LABELLED){
        char* token = strtok(buff[0], delim);
        int j = 0;
        while(token != NULL){
            ret->column_names[j] = copy_string(token);
            if (strlen(token) > ret->formatting[j]){
                ret->formatting[j] = strlen(token);
            }
            j++;
            token = strtok(NULL, delim);
            
        }
    }

    int initial_i = ret->column_names_exist;
    for(i=0; i<lines-initial_i; i++){
        ret->df[i] = create_empty_series(datatypes, num_columns);
        char* token = strtok(buff[i+initial_i], delim);
        int j=0;
        while(token != NULL){
            switch(ret->datatypes[j]){
                case INT: ret->df[i]->set(ret->df[i], j, integer(atoi(token)));
                    if(ceil(log10(atoi(token))) > ret->formatting[j]){
                        ret->formatting[j] = ceil(log10(atoi(token)));
                    }
                    break;
                case STRING: ret->df[i]->set(ret->df[i], j, copy_string(token));
                    if (strlen(token) > ret->formatting[j]){
                        ret->formatting[j] = strlen(token);
                    }
                    break;
                case DOUBLE: ret->df[i]->set(ret->df[i], j, doub(strtod(token, NULL))); break;
                default:; printf("Error on Row: %d\n", i); assert(0);
            }
            j++;
            token = strtok(NULL, delim);
        }
    }

    ret->size = &df_size;
    ret->columns = &df_columns;
    ret->dtypes = &dataframe_dtypes;
    ret->head = &dataframe_print_head;
    ret->tail = &dataframe_print_tail;
    ret->drop_columns = &df_delete_columns;
    ret->rename_column = &df_column_rename;

    ret->resize = &df_resize;
    ret->nunique_col = &df_nunique_col;

    ret->print_conditional = &dataframe_print_condition;
    ret->swapaxes= &df_colswap;

    ret->merge = &df_applymerge;
    ret->print_col_freq = &df_frequency_table_col;
    ret->mean = &df_mean;
    
    return ret;

    
}

static int series_set(series_t* s, int col_num, void* data);
static void* series_get(series_t* s, int col_num);
static void series_del(series_t* s, int col_num);
static int destroy_series(series_t* s);
static void series_resize(series_t* s, int new_size);
static int series_colswap(series_t* s, int c1, int c2);

series_t* create_empty_series(int* datatypes, int n)
{
    series_t* ret = malloc(sizeof(*ret));
    ret->series = malloc(n * sizeof(*ret->series));
    ret->datatypes = malloc(n * sizeof(*datatypes));
    assert(unwanted_null(ret));
    assert(unwanted_null(ret->series));
    assert(unwanted_null(ret->datatypes));

    ret->num_columns = n;
    ret->alloc_columns = n;
    int i;
    for(i=0; i<n; i++){
        ret->datatypes[i] = datatypes[i];
        ret->series[i] = NULL;
    }

    ret->set = &series_set;
    ret->get = &series_get;
    ret->del = &series_del;
    ret->resize = &series_resize;
    ret->col_swap = &series_colswap;
    ret->destroy = &destroy_series;
    return ret;
}

static int series_set(series_t* s, int col_num, void* data)
{
    if (col_num > s->num_columns){
        assert(0 && "Column Num exceeds the number of columns in Series");
    }
    s->series[col_num] = data;
    return SUCCESSFULLY_SET_SERIES_ENTRY;
}

static void* series_get(series_t* s, int col_num)
{
    if (col_num > s->num_columns){
        assert(0 && "Column Num exceeds the number of columns in Series");
    }
    return s->series[col_num];
}
int print_series(series_t* s, int mode);
static void series_del(series_t* s, int col_num)
{
    if (col_num > s->num_columns){
        assert(0 && "Column Num exceeds the number of columns in Series");
    }
    
    free(s->series[col_num]);
    int i;
    for(i=col_num; i<s->num_columns-1; i++){
        s->series[i] = s->series[i+1];
        s->datatypes[i] = s->datatypes[i+1];
    }

    s->num_columns--;
    return;
}


static int series_colswap(series_t* s, int c1, int c2)
{
    if (c1 > s->num_columns || c2 > s->num_columns){
        return FAILURE_COL_NUM_EXCEED_SERIES_LEN;
    }
    scalar_swap(&s->series[c1], &s->series[c2], sizeof(void*));
    scalar_swap(&s->datatypes[c1], &s->datatypes[c2], sizeof(int*));
    return 102;
}

int print_series(series_t* s, int mode)
{
    int i;
    for(i=0; i<s->num_columns; i++){
        switch(s->datatypes[i]){
            case INT: printf("%10d ", *((int*)s->series[i])); break;
            case STRING: printf("%10s ", ((char*)s->series[i])); break;
            case DOUBLE: ;
                if (*((double*)s->series[i]) > 0){
                    printf("%8.5lf ", *((double*)s->series[i]));
                }
                else{
                    printf("%6.5lf ", *((double*)s->series[i]));
                }
                break;
            default: printf("%d\n", (s->datatypes[i])); assert(0);
        }
    }
    printf("\n");
    return 100;
}

static void series_resize(series_t* s, int new_size)
{
    assert(s != NULL);
    /* This function only resizes upwards, not downwards */
    if(s->alloc_columns >= new_size){
        return;
    }
    s->alloc_columns = new_size;
    s->series = realloc(s->series, s->alloc_columns * sizeof(*s->series));
    s->datatypes = realloc(s->datatypes, s->alloc_columns * sizeof(*s->datatypes));
    assert(unwanted_null(s->series));
    assert(unwanted_null(s->datatypes));
}

int print_datatypes(series_t* s)
{
    int i;
    for(i=0; i<s->num_columns; i++){
        switch(s->datatypes[i]){
            case INT: printf("Integer\n"); break;
            case STRING: printf("String\n"); break;
            case DOUBLE: printf("Double\n"); break;
            default: assert(0);
        }
    }
    return 105;
}

static int destroy_series(series_t* s)
{
    int i;

    printf("Destroying: %d columns\n", s->num_columns);
    for(i=0; i<s->alloc_columns; i++){
        free(s->series[i]);
    }
    free(s->datatypes);
    free(s->series);
    free(s);
    return 110;
}

static void dataframe_dtypes(dataframe_t* df)
{
    assert(df != NULL);
    int i;
    for(i=0; i<df->num_columns; i++){
        switch(df->datatypes[i]){
            case INT: printf("Integer\n"); break;
            case STRING: printf("String\n"); break;
            case DOUBLE: printf("Double\n"); break;
            default: assert(0);
        }
    }
}
static void df_print_help(dataframe_t* df, int row){
    int i;
    for(i=0; i<df->num_columns; i++){
        int width = df->formatting[i];
        switch(df->datatypes[i]){
            case INT: printf("%*d ", width, *((int*)df->df[row]->series[i])); break;
            case STRING: printf("%*s ", width, ((char*)df->df[row]->series[i])); break;
            case DOUBLE: ;
                if (*((double*)df->df[row]->series[i]) > 0){
                    printf("%*lf ", width, *((double*)df->df[row]->series[i]));
                }
                else{
                    printf("%*lf ", width, *((double*)df->df[row]->series[i]));
                }
                break;
            default: printf("%d\n", (df->datatypes[i])); assert(0);
        }
    }
    printf("\n");
}

static int column_name_index(dataframe_t* df, char* col_name)
{
    if (df->column_names_exist == NOT_LABELLED){
        assert(0);
    }
    int index = lsearch(df->column_names, df->num_columns, col_name);
    if (index < 0){
        fprintf(stderr, "No such Column Name: %s\n", col_name);
        assert(0);
    }
    return index;
    
}
static void dataframe_print_condition(dataframe_t* df, char* col_name, void* condition)
{
    int i;
    if (df->column_names_exist == LABELLED){
        for(i=0; i<df->num_columns; i++){
            printf("%*s ", df->formatting[i], df->column_names[i]);
        }
        printf("\n");
    }
    int index = column_name_index(df, col_name);
    for(i=0; i<df->num_rows; i++){
        int unsatisfied = 1;
        void* data = df->df[i]->series[index];
        switch(df->datatypes[index]){
            case INT: unsatisfied = int_cmp(data, condition); break;
            case STRING: unsatisfied = str_cmp(data, condition); break;
            case DOUBLE: unsatisfied = double_cmp(data, condition); break;
            default: assert(0);
        }
        if (!unsatisfied){
            df_print_help(df, i);
        }
    }
    free(condition);
}
static void dataframe_print_head(dataframe_t* df, int n)
{
    assert(df != NULL);
    printf("Shape: %d x %d\n", df->num_rows, df->num_columns);
    int i;
    if (df->column_names_exist == LABELLED){
        for(i=0; i<df->num_columns; i++){
            printf("%*s ", df->formatting[i], df->column_names[i]);
        }
        printf("\n");
    }
    for(i=0; i<n; i++){
        df_print_help(df, i);
    }
    printf("\n");
}

static void dataframe_print_tail(dataframe_t* df, int n)
{
    assert(df != NULL);
    printf("Shape: %d x %d\n", df->num_rows, df->num_columns);
    int i;
    if (df->column_names_exist == LABELLED){
        for(i=0; i<df->num_columns; i++){
            printf("%*s ", df->formatting[i], df->column_names[i]);
        }
        printf("\n");
    }
    for(i=df->num_rows-n; i<df->num_rows; i++){
        df_print_help(df, i);
    }
    printf("\n");
}

#if 0
static void dataframe_print(dataframe_t* df)
{
    int i;
    printf("Shape: %d x %d\n", df->num_rows, df->num_columns);
    if (df->column_names_exist == LABELLED){
        for(i=0; i<df->num_columns; i++){
            printf("%*s ", df->formatting[i], df->column_names[i]);
        }
        printf("\n");
    }
    for(i=0; i<df->num_rows; i++){
        df_print_help(df, i);
    }
    printf("\n");
}
#endif

void dataframe_print_formatting(dataframe_t* df)
{
    assert(df != NULL);
    int i;
    for(i=0; i<df->num_columns; i++){
        printf("%d\n", df->formatting[i]);
    }
}

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_columns
 *
 * Arguments: dataframe
 *
 * Returns: dataframe rows * columns
 *
 * Dependency: None
 */
static int df_size(dataframe_t* df)
{
    assert(df != NULL);
    return df->num_rows * df->num_columns;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_columns
 *
 * Arguments: dataframe
 *
 * Returns: Void (prints all column names of dataframe)
 *
 * Dependency: None
 */
static void df_columns(dataframe_t* df)
{
    assert(df != NULL);
    if (df->column_names_exist == NOT_LABELLED){
        fprintf(stderr, "No column names associated\n");
        assert(0 && "No Column Names");
    }
    int i;
    for(i=0; i<df->num_columns; i++){
        printf("%d: %s\n", i, df->column_names[i]);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_nunique_col
 *
 * Arguments: dataframe
 *            column_name to be deleted
 *
 * Returns: Void (deletes column from dataframe)
 *
 * Dependency: utils.h
 *             column_name_index
 *             series_del
 */
static void df_delete_column(dataframe_t* df, char* col_name)
{
    assert(df != NULL);
    assert(col_name != NULL);
    int i;

    int index = column_name_index(df, col_name);

    for(i=0; i<df->num_rows; i++){
        df->df[i]->del(df->df[i], index);
    }
    free(df->column_names[index]);
    for(i=index; i<df->num_columns-1; i++){
        df->column_names[i] = df->column_names[i+1];
        df->formatting[i] = df->formatting[i+1];
        df->datatypes[i] = df->datatypes[i+1];
    }
    df->num_columns--;
    return;
}
//-----------------------------------------------------------------------------


/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_delete_columns
 *
 * Arguments: dataframe
 *            column names
 *            number of columns to be deleted
 *
 * Returns: Number of unique entries in column
 *
 * Dependency: df_delete_column
 */
static void df_delete_columns(dataframe_t* df, char** col_names, int n)
{
    assert(df != NULL);
    assert(col_names != NULL);
    if (df->column_names_exist == NOT_LABELLED){
        fprintf(stderr, "No column names associated\n");
        assert(0 && "No Column Names");
    }
    
    int i;
    for(i=0; i<n; i++){
        df_delete_column(df, col_names[i]);
    }
}
//-----------------------------------------------------------------------------

#if 0
static void* dataframe_mode_col(dataframe_t* df, char* col_name, int dropna)
{
    assert(df != NULL);
    assert(col_name != NULL);
    int index = column_name_index(df, col_name);
    hashtable_t* h = create_simple_hashtable(df->num_rows, df->datatypes[index]);
    int i;
    for(i=0; i<df->num_rows; i++){
        h->insert(h, scalar_copy(df->df[i]->series[index], df->datatypes[index]) , NULL);
    }
    void* ret = scalar_copy(h->most_frequent(h), df->datatypes[index]);
    h->destroy(h);
    return ret;
}
#endif

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_nunique_col
 *
 * Arguments: dataframe
 *            column_name
 *            whether to drop missing values (TODO)
 *
 * Returns: Number of unique entries in column
 *
 * Dependency: utils.h
 *             hashtable.h
 *             column_name_index
 */
static int df_nunique_col(dataframe_t* df, char* col_name, int dropna)
{
    assert(df != NULL);
    assert(col_name != NULL);
    int index = column_name_index(df, col_name);
    int dtype = df->datatypes[index];
    hashtable_t* h = create_simple_hashtable(df->num_rows, dtype);
    int i;
    for(i=0; i<df->num_rows; i++){
        h->insert(h, scalar_copy(df->df[i]->series[index], dtype) , NULL);
    }
    int num_unique = h->len(h);
    h->destroy(h);
    return num_unique;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_mean
 *
 * Arguments: dataframe
 *            axis to calculate mean of (ROW, COLUMN)
 *            column name or row name as desired
 *             (if no row names set, then "0" means row 0)
 *
 * Returns: Void
 *
 * Dependency: utils.h
 *             column_name_index
 */
static double df_mean(dataframe_t* df, int axis, char* name)
{
    
    double sum = 0.0;
    int i;
    if (axis == COLUMN){
        int index = column_name_index(df, name);
        if (is_numeric(df->datatypes[index])){
            for(i=0; i<df->num_rows; i++){
                sum += *((int*)df->df[i]->series[index]);
            }
            return sum/df->num_rows;
        }
        else{
            return 0.0;
        }
    }
    else if (axis == ROW){
        /* TODO */
        return 100.1;
    }
    else{
        assert(0 && "Axis not recognised, try COLUMN or ROW");
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_frequency_table_col
 *
 * Arguments: dataframe
 *            name of column you want frequencies of
 *
 * Returns: Void (prints frequency table of column name)
 *
 * Dependency: utils.h
 *             series_colswap
 */
static void df_frequency_table_col(dataframe_t* df, char* col_name)
{
    assert(df != NULL);
    assert(col_name != NULL);
    int index = column_name_index(df, col_name);
    hashtable_t* h = create_simple_hashtable(df->num_rows, df->datatypes[index]);
    int i;
    for(i=0; i<df->num_rows; i++){
        h->insert(h, scalar_copy(df->df[i]->series[index], df->datatypes[index]) , NULL);
    }
    h->print_frequencies(h, df->formatting[index]);
    h->destroy(h);
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_applymerge
 *
 * Arguments: dataframe
 *            name of first column (merge into this)
 *            name of second column
 *            mode of merging two columns (ADDITION, SUBTRACTION)
 *
 * Returns: Void (merges second  with first, drops second)
 *          Only works if both columns have numeric data
 * 
 * Dependency: utils.h
 *             column_name_index
 *             df_delete_columns
 *             
 */
static void df_applymerge(dataframe_t* df, char* col_name1, char* col_name2, int mode)
{
    int index1 = column_name_index(df, col_name1);
    int index2 = column_name_index(df, col_name2);
    int* type1 = &df->datatypes[index1];
    int* type2 = &df->datatypes[index2];
    if (!is_numeric(*type1) || !is_numeric(*type2)){
        fprintf(stderr, "Cannot merge columns as they need to both be numeric: %d, %d\n", *type1, *type2);
        assert(0);
        return;
    }
    int i;
    int datatype_modified;
    for(i=0; i<df->num_rows; i++){
        void* a = df->df[i]->series[index1];
        void* b = df->df[i]->series[index2];
        switch(mode){
            case ADDITION: datatype_modified = addition(a, b, *type1, *type2); break;
            case SUBTRACTION: datatype_modified = subtraction(a, b, *type1, *type2); break;
        }
    }
    if (datatype_modified){
        scalar_swap(type1, type2, sizeof(*type1));
    }
    df->drop_columns(df, &col_name2, 1);
    return;
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_resize
 *
 * Arguments: dataframe
 *            new (increased) number of columns
 *
 * Returns: Void
 *
 * Dependency: utils.h
 *             series_resize
 */
static void df_resize(dataframe_t* df, int new_size)
{
    assert(df != NULL);
    /* This function only resizes upwards, not downwards */
    if(df->alloc_columns >= new_size){
        return;
    }
    df->alloc_columns = new_size;
    df->formatting = realloc(df->formatting,
                              df->alloc_columns * sizeof(*df->formatting));
    df->datatypes = realloc(df->datatypes,
                             df->alloc_columns * sizeof(*df->datatypes));
    df->column_names = realloc(df->column_names,
                             df->alloc_columns * sizeof(*df->column_names));
    assert(unwanted_null(df->formatting));
    assert(unwanted_null(df->datatypes));
    assert(unwanted_null(df->column_names));
    int i;
    for(i=0; i<df->num_rows; i++){
        df->df[i]->resize(df->df[i], new_size);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_column_rename
 *
 * Arguments: dataframe
 *            name of column to be renamed
 *            new name desired
 *
 * Returns: Void
 *
 * Dependency: None
 */
static void df_column_rename(dataframe_t* df, char* old_name, char* new_name)
{
    assert(df != NULL);
    if (df->column_names_exist == NOT_LABELLED){
        fprintf(stderr, "No column names associated\n");
        assert(0 && "No Column Names");
    }
    int index = lsearch(df->column_names, df->num_columns, old_name);
    if (index < 0){
        fprintf(stderr, "No such Column Name: %s\n", old_name);
        assert(0);
    }
    char* old = df->column_names[index];
    df->column_names[index] = copy_string(new_name);
    free(old);
    if (strlen(new_name) > df->formatting[index]){
        df->formatting[index] = strlen(new_name);
    }
}
//-----------------------------------------------------------------------------

/*****************************************************************************/
/**----------------------------------------------------------------------------
 * Function: df_colswap
 *
 * Arguments: dataframe
 *            index of c1
 *            index of c2
 *
 * Returns: Void
 *
 * Dependency: utils.h
 *             series_colswap
 */
static void df_colswap(dataframe_t* df, int c1, int c2)
{
    if (c1 > df->num_columns || c2 > df->num_columns){
        assert(0 && "Column Num exceeds the number of columns in dataframe");
    }
    else if (c1 <= 0 || c2 <= 0){
        assert(0 && "Cannot have negative column numbers");
    }

    int i;
    for(i=0; i<df->num_rows; i++){
        df->df[i]->col_swap(df->df[i],c1, c2);
    }
    scalar_swap(&df->datatypes[c1], &df->datatypes[c2], sizeof(int*));
    scalar_swap(&df->formatting[c1], &df->formatting[c2], sizeof(int*));
    scalar_swap(&df->column_names[c1], &df->column_names[c2], sizeof(char*));
}
//-----------------------------------------------------------------------------

#if 0
static void dataframe_append_winrate(dataframe_t* df, char* w, char* l)
{
    assert(df != NULL);
    int winner_index = column_name_index(df, w);
    int loser_index = column_name_index(df, l);

    if ((df->num_columns + 2) > df->alloc_columns){
        df->resize(df, df->alloc_columns + 2);
    }

    char* winner_wr_col = string_concatenate(w, WR_POSTPEND);
    char* loser_wr_col = string_concatenate(l, WR_POSTPEND);
    df->column_names[df->num_columns] = winner_wr_col;
    df->column_names[df->num_columns + 1] = loser_wr_col;
    df->datatypes[df->num_columns] = DOUBLE;
    df->datatypes[df->num_columns + 1] = DOUBLE;
    df->formatting[df->num_columns] = strlen(winner_wr_col);
    df->formatting[df->num_columns + 1] = strlen(loser_wr_col);

    df->num_columns += 2;
    
    int winner_wr_index = column_name_index(df, winner_wr_col);
    int loser_wr_index = column_name_index(df, loser_wr_col);

    hashtable_t* winner = create_simple_hashtable(df->nunique_col(df, w, 0), STRING);
    hashtable_t* loser = create_simple_hashtable(df->nunique_col(df, l, 0), STRING);
    
    int i;
    for(i=0; i<df->num_rows; i++){
        char* win = df->df[i]->series[winner_index];
        char* lose = df->df[i]->series[loser_index];
        df->df[i]->num_columns += 2;

        int winner_wins = winner->get_frequency(winner, win);
        int winner_losses = loser->get_frequency(loser, win);
        int loser_wins = winner->get_frequency(winner, lose);
        int loser_losses = loser->get_frequency(loser, lose);

        winner->insert(winner, copy_string(win), NULL);
        loser->insert(loser, copy_string(lose), NULL);
        if (winner_wins == 0 || winner_losses == 0){
            df->df[i]->set(df->df[i], winner_wr_index, doub(1.0));
        }
        else{
            double* wr = doub(((double)winner_wins / (winner_wins + winner_losses))*100);
            df->df[i]->set(df->df[i], winner_wr_index, wr);
        }
        if (loser_wins == 0 || loser_losses == 0){
            df->df[i]->set(df->df[i], loser_wr_index, doub(0.0));
        }
        else{
            double* wr = doub(((double)loser_wins / (loser_wins + loser_losses))*100);
            df->df[i]->set(df->df[i], loser_wr_index, wr);
            
        }
    }

    winner->destroy(winner);
    loser->destroy(loser);
}
#endif

#if 0
static void dataframe_append_elo(dataframe_t* df, char* winner, char* loser,
                                 int starting_elo, double K, int mode)
{
    assert(df != NULL);
    int winner_index = column_name_index(df, winner);
    int loser_index = column_name_index(df, loser);

    if ((df->num_columns + 2) > df->alloc_columns){
        df->resize(df, df->alloc_columns + 2);
    }
    hashtable_t* h = create_simple_hashtable(df->nunique_col(df, "Loser", 0), STRING);

    char postpend[] = ELO_POSTPEND;
    size_t winner_col_size = strlen(winner) + strlen(postpend) + 1;
    size_t loser_col_size = strlen(loser) + strlen(postpend) + 1;

    char* winner_elo_col = malloc(winner_col_size * sizeof(char*));
    char* loser_elo_col = malloc(loser_col_size * sizeof(char*));
    assert(unwanted_null(winner_elo_col));
    assert(unwanted_null(loser_elo_col));
    memcpy(winner_elo_col, winner, strlen(winner));
    memcpy(winner_elo_col + strlen(winner), postpend, strlen(postpend) + 1);
    memcpy(loser_elo_col, loser, strlen(loser));
    memcpy(loser_elo_col + strlen(loser), postpend, strlen(postpend) + 1);
    df->column_names[df->num_columns] = winner_elo_col;
    df->column_names[df->num_columns + 1] = loser_elo_col;
    df->datatypes[df->num_columns] = INT;
    df->datatypes[df->num_columns + 1] = INT;
    df->formatting[df->num_columns] = strlen(winner_elo_col);
    df->formatting[df->num_columns + 1] = strlen(loser_elo_col);

    df->num_columns += 2;
    
    int winner_elo_index = column_name_index(df, winner_elo_col);
    int loser_elo_index = column_name_index(df, loser_elo_col);

    int i;
    for(i=0; i<df->num_rows; i++){
        char* winner = df->df[i]->series[winner_index];
        char* loser = df->df[i]->series[loser_index];
        
        
        /* If Winner or Loser of Match is not in dictionary, add them and set their elo
         * to starting_elo
         */
        if (h->in(h, winner) == NULL){
            int* start_elo = malloc(sizeof(*start_elo));
            assert(unwanted_null(start_elo));
            *start_elo = starting_elo;
            h->insert(h, copy_string(winner), start_elo);
        } 

        if (h->in(h, loser) == NULL){
            int* start_elo = malloc(sizeof(*start_elo));
            assert(unwanted_null(start_elo));
            *start_elo = starting_elo;
            h->insert(h, copy_string(loser), start_elo);
        }

        df->df[i]->num_columns += 2;
        df->df[i]->set(df->df[i], winner_elo_index, integer(*((int*)h->in(h, winner))));
        df->df[i]->set(df->df[i], loser_elo_index, integer(*((int*)h->in(h, loser))));

        elo_rating(h->in(h, winner), h->in(h, loser), K, 1);
        
        
        df->df[i]->datatypes[winner_elo_index] = INT;
        df->df[i]->datatypes[loser_elo_index] = INT;
    }

}
#endif

const void** dataframe_column_iterator(dataframe_t* df, char* col_name, int mode)
{
    assert(df != NULL);
    int index = column_name_index(df, col_name);
    int i;

    const void** ret = malloc(df->num_rows * sizeof(*ret));
    for(i=0; i<df->num_rows; i++){
        ret[i] = df->df[i]->series[index];
    }
    return ret;
}