#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>


void add_process(pid_t pid, const char* name);
const char *get_process_name(pid_t pid);
void remove_process(pid_t pid);
void update_process_status(pid_t pid, const char* status);
void execute_ping(char* token, int* from_history);
void execute_activities(int* from_history);
void execute_bg(char* token, int* from_history);
void system_command_execute(char* command, char* process_info);
void execute_fg(char* token, int* from_history);

#endif