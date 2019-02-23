#ifndef FILES_H
#define FILES_H

int lines_in_file(char* fname);
int columns_in_file(char* fname, char* delim);
int read_file(char* fname, char** buff, int num_rows);
int copy_file(char* src, char* dest);

#endif // FILES_H