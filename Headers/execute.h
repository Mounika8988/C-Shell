#ifndef _EXECUTE_H
#define _EXECUTE_H

#include "headers.h"

// void execute_my_command(char* real_command, char* token, int* from_history, char* previous_dir, char* home, char** log_commands, int* command_count, int* exit_flag, char* command, char* process_info, int* process_info_flag);
void execute_my_command(char* real_command, char* token, int* from_history, char* previous_dir, char* home, char** log_commands, int* command_count, char* command, char* process_info, int* process_info_flag);   
void execute(char** commands_by_and, int* from_history, char* previous_dir, char* home, char** log_commands, int* command_count, char* command, char* process_info, int* process_info_flag, int j);
bool isSystemCommand(char* command, char* home);
void execute_syscommand_with_pipe(int k, int pipe_count, int* pipefd, char** commands_by_pipe, char* input_file, char* output_file, bool append_mode, int* prev_fd, int* fd_null, int fd_null_0, char* process_info);
void execute_my_command_with_pipe(int k, int pipe_count, int* pipefd, char* input_file, char* output_file, int* prev_fd, bool append_mode, int* fd_null, int fd_null_0, char** commands_by_pipe, int* from_history, char* previous_dir, char* home, char** log_commands, int* command_count, char* command, char* process_info, int* process_info_flag);

// execute_my_command_with_pipe(k, pipe_count, pipefd, intput_file, output_file, &prev_fd, append_mode, &fd_null, fd_null_0, commands_by_pipe, &from_history, previous_dir, home, log_commands, &command_count, command, process_info, &process_info_flag);             




#endif