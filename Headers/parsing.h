#ifndef _PARSING_H_
#define _PARSING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void get_file_names(char** commands_by_pipe, char** input_file, char** output_file, bool* append_mode, int k);
void parse_by_and(char** command_without_spaces, char** commands_by_and, int* and_count, int i);

#endif