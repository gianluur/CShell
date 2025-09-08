#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void enable_raw_mode();
void disable_raw_mode();
void print_current_directory();
void reverse_line(char *line);
long get_file_size(FILE *file);

#endif