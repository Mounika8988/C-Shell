#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <linux/limits.h>

char** initialize_log_commands(char* home, int *command_count);
void update_log(char** log_commands, char* home, int command_count);
void add_command(char** log_commands, char* command, int *command_count);
void execute_log(char* token, char** log_commands, int* command_count, char* home, char* command, int* from_history);



#endif