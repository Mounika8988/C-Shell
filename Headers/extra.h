#ifndef _EXTRA_H_
#define _EXTRA_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <ctype.h>
#include <stdbool.h>



void trim_and_remove_extra_spaces(char *input, char *output);
char* relative_path(char* home, char* path);
int has_substring(char* str, char* substr);
void process_name(int pid, char* pname);   
void replace_start(char *A, const char *B, const char *C);
void add_spaces(char *input, char *output);
void redirect_input(char* input_file);
void redirect_output(char* output_file, bool append_mode);
void parse_and_print_html(char *html_content);
void initialize_file_names(char** commands_by_pipe, char** input_file, char** output_file, bool* append_mode, int k);
void free_array(char** array, int size);    
int compare_strings(const void *a, const void *b);



#endif