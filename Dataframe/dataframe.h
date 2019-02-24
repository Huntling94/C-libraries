#ifndef DATAFRAME_H
#define DATAFRAME_H

#define LABELLED 1
#define NOT_LABELLED 0

#define ROW 0
#define COLUMN 1

typedef struct dataframe dataframe_t;
typedef struct series series_t;

struct series{
    void** series;
    int* datatypes;
    int num_columns;
    int alloc_columns;

    int (*set)(series_t* s, int col_num, void* data);
    void* (*get)(series_t* s, int col_num);
    void (*del)(series_t* s, int col_num);
    void (*resize)(series_t* s, int new_size);
    int (*col_swap)(series_t* s, int c1, int c2);
    int (*destroy)(series_t* s);
};

series_t* create_empty_series(int* datatypes, int n);


struct dataframe{
    series_t** df;
    int* datatypes;
    int* formatting;

    int num_rows;
    int num_columns;
    int alloc_rows;
    int alloc_columns;

    char** column_names;
    int column_names_exist;

    int (*size)(dataframe_t* df);
    void (*columns)(dataframe_t* df);
    void (*dtypes)(dataframe_t* df);
    void (*head)(dataframe_t* df, int n);
    void (*tail)(dataframe_t* df, int n);
    void (*drop_columns)(dataframe_t* df, char** col_names, int n);
    void (*rename_column)(dataframe_t* df, char* old_name, char* new_name);

    void (*resize)(dataframe_t* df, int new_size);
    int (*nunique_col)(dataframe_t* df, char* col_name, int dropna);
    void (*print_conditional)(dataframe_t* df, char* col_name, void* condition);

    void(*swapaxes)(dataframe_t*, int a1, int a2);

    void (*merge)(dataframe_t*, char* col_name1, char* col_name2, int mode);
    void (*print_col_freq)(dataframe_t* df, char* col_name);
    double (*mean)(dataframe_t* df, int axis, char* col_name);
};

dataframe_t* csv_to_dataframe(char* fname, char* delim, int* datatypes, int columns_named);

#endif // DATAFRAME_h