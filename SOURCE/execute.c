#include "headers.h"

void execute_my_command(char* real_command, char* token, int* from_history, char* previous_dir, char* home, char** log_commands, int* command_count, char* command, char* process_info, int* process_info_flag){

    if(real_command == NULL){
        real_command = token;
    }

    if(strcmp(token, "hop") == 0 || strcmp(real_command, "hop") == 0){
        execute_hop(token, previous_dir, home, from_history);
    }
    else if(strcmp(token, "reveal") == 0 || strcmp(real_command, "reveal") == 0){
        execute_reveal(token, from_history, previous_dir, home);
    }
    else if(strcmp(token, "log") == 0 || strcmp(real_command, "log") == 0){
        execute_log(token, log_commands, command_count, home, command, from_history);
    }  
    else if(strcmp(token, "proclore") == 0 || strcmp(real_command, "proclore") == 0){
        execute_proclore(token, from_history);          
    }       
    else if(strcmp(token, "seek") == 0 || strcmp(real_command, "seek") == 0){
        execute_seek(token, from_history);            
    }
    else if(strcmp(token, "iMan") == 0 || strcmp(real_command, "iMan") == 0){
        execute_iMan(token, from_history);
    }
    else if(strcmp(token, "neonate") == 0 || strcmp(real_command, "neonate") == 0){
        execute_neonate(token, from_history);
    }   
    else if(strcmp(token, "activities") == 0 || strcmp(real_command, "activities") == 0 ){
        execute_activities(from_history);
    }   
    else if(strcmp(token, "ping") == 0 || strcmp(real_command, "ping") == 0){
        execute_ping(token, from_history);
    }
    else if(strcmp(token, "fg") == 0 || strcmp(real_command, "fg") == 0){
        execute_fg(token, from_history);
    }
    else if(strcmp      (token, "bg") == 0 || strcmp(real_command, "bg") == 0){
        execute_bg(token, from_history);
    }

    return; 
}

bool isSystemCommand(char* command, char* home){

    char temp_command[MAX_COMMAND_LENGTH];
    strcpy(temp_command, command);
    char* token = strtok(temp_command, " ");
    if(token != NULL){
        if(strcmp(token, "hop") == 0 ||     
            strcmp(token, "reveal") == 0 ||     
            strcmp(token, "log") == 0 || 
            strcmp(token, "proclore") == 0 || 
            strcmp(token, "seek") == 0 ||
            strcmp(token, "iMan") == 0 ||
            strcmp(token, "neonate") == 0 ||
            strcmp(token, "activities") == 0 ||
            strcmp(token, "ping") == 0 ||
            strcmp(token, "fg") == 0 ||
            strcmp(token, "bg") == 0){    
            return false;
        }
        else{
            char* real_command = NULL;
            char temp_token[STRLEN_MAX];    
            strcpy(temp_token, token);
            real_command = find_alias(temp_token, home);

            if(real_command != NULL){
                return false;
            }
            else{
                return true;
            }
        }
    }
    else{
        return false; 
    }
}


void execute(char** commands_by_and, int* from_history, char* previous_dir, char* home, char** log_commands, int* command_count, char* command, char* process_info, int* process_info_flag, int j){
    char* token = strtok(commands_by_and[j], " ");
    if(token != NULL){
        if(strcmp(token, "hop") == 0 ||     
            strcmp(token, "reveal") == 0 ||     
            strcmp(token, "log") == 0 || 
            strcmp(token, "proclore") == 0 || 
            strcmp(token, "seek") == 0 ||
            strcmp(token, "iMan") == 0 ||
            strcmp(token, "neonate") == 0 ||
            strcmp(token, "activities") == 0 ||
            strcmp(token, "ping") == 0 ||
            strcmp(token, "fg") == 0 ||
            strcmp(token, "bg") == 0){    
            execute_my_command(NULL, token, from_history, previous_dir, home, log_commands, command_count, command, process_info, process_info_flag); 
        }
        else{   
            if(*from_history == 1){  
                *from_history = 0;
            }               

            char* real_command = NULL;
            char temp_token[STRLEN_MAX];    
            strcpy(temp_token, token);
            real_command = find_alias(temp_token, home);

            if(real_command != NULL){
                execute_my_command(real_command, token, from_history, previous_dir, home, log_commands, command_count, command, process_info, process_info_flag);
                free(real_command);
            }
            else{
                char system_command[MAX_COMMAND_LENGTH];                    
                strcpy(system_command, token);
                token = strtok(NULL, " ");
                while(token != NULL){   
                    strcat(system_command, " ");
                    strcat(system_command, token);
                    token = strtok(NULL, " ");
                }     

                // if(pipe_count > 1){
                // }  
                // else{              
                system_command_execute(system_command, process_info);
                if(process_info[0] != '\0'){
                    *process_info_flag = 1;
                }                
            }   
        }                  
    }
    else{
        return; 
    }
}

void execute_syscommand_with_pipe(int k, int pipe_count, int* pipefd, char** commands_by_pipe, char* input_file, char* output_file, bool append_mode, int* prev_fd, int* fd_null, int fd_null_0, char* process_info){
    if (k < pipe_count - 1) {
        pipe(pipefd);
    }

    char* myargs[4096];
    char return_message[4096];
    int is_background = 0;

    if (commands_by_pipe[k][strlen(commands_by_pipe[k]) - 1] == '&') {
        is_background = 1;
        commands_by_pipe[k][strlen(commands_by_pipe[k]) - 1] = '\0';
    }

    time_t start, end;
    double elapsed;
    int elapsed_int;
    start = time(NULL);

    char* token = strtok(commands_by_pipe[k], " ");
    int i = 0;        
    // while(token != NULL){
    //     myargs[i] = token;
    //     i++;
    //     token = strtok(NULL, " ");
    // }
    // myargs[i] = NULL;

    int in_quotes = 0;
    char* arg = NULL;    

    while (token != NULL) {
        if (token[0] == '"') {
            // Start of a quoted string
            in_quotes = 1;
            arg = strdup(token + 1); // Skip the opening quote
            if (strchr(arg, '"')) {
                // Handle closing quote in the same token
                arg[strlen(arg) - 1] = '\0'; // Remove the ending quote
                myargs[i++] = arg;
                in_quotes = 0;
            }
            
        } else if (in_quotes) {
            // Inside a quoted string
            if (strchr(token, '"')) {
                // Handle closing quote in the same token
                token[strlen(token) - 1] = '\0'; // Remove the ending quote
                arg = realloc(arg, strlen(arg) + strlen(token) + 1);
                strcat(arg, " ");
                strcat(arg, token);
                myargs[i++] = arg;
                in_quotes = 0;
            } else {
                // Continue accumulating the quoted string
                arg = realloc(arg, strlen(arg) + strlen(token) + 2);
                strcat(arg, " ");
                strcat(arg, token);
            }
        } else {
            // Regular token
            myargs[i++] = strdup(token);
        }
        token = strtok(NULL, " ");
    }
    myargs[i] = NULL; // Null-terminate the argument array

    pid_t pid = fork();

    if (pid == 0) {
        if(input_file){
            int fd = open(input_file, O_RDONLY);
            if(fd < 0) {
                perror("Error opening input file");
                exit(1);    
            }
            dup2(fd, 0);
            close(fd);  
        }                   
        else if(*prev_fd != -1) {  // Not the first command
            dup2(*prev_fd, 0);  // Take input from the previous command
            close(*prev_fd);  // Close the previous command's read end
        }

        if(output_file && k == pipe_count - 1) { // last command has output file
            int fd;
            if (append_mode) fd = open(output_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
            else fd = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

            if(fd < 0) {
                perror("Error opening output file");
                exit(1);
            }
            dup2(fd, 1);
            close(fd);
        }
        else if(output_file && k < pipe_count - 1 ){  // Not the last command but has output file
            int fd;
            if (append_mode) fd = open(output_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
            else fd = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

            if(fd < 0) {
                perror("Error opening output file");
                exit(1);
            }
            dup2(fd, 1);
            close(fd);

            dup2(pipefd[1], *fd_null);
            close(pipefd[0]);

        }
        else if(!output_file && k < pipe_count - 1) {  // Not the last command and no output file
            dup2(pipefd[1], 1);  // Send output to the next command 
            close(pipefd[0]);  // Close the read end of the pipe
        }

        close(pipefd[1]);

        if(execvp(myargs[0], myargs) == -1){
            perror("Invalid command");
            exit(1);
        }
    }
    else if(pid > 0){
        if(is_background == 1){
            add_process(pid, myargs[0]);    
            // wait(NULL);
            printf("%d\n", pid);
        }
        else{
            add_process(pid, myargs[0]);
            foreground_pid = pid;
            int status;
            waitpid(pid, &status, WUNTRACED);  // wait for child process to finish
            foreground_pid = -1;
            if (WIFSTOPPED(status)) {   
                update_process_status(pid, "Stopped");
            }
            else if(WIFSIGNALED(status) && WTERMSIG(status) == SIGINT){
                printf("\n");
            } else {
                remove_process(pid);
            }

            end = time(NULL);
            elapsed = difftime(end, start);
            elapsed_int = (int)elapsed;

            if(elapsed_int > 2){
                snprintf(return_message, sizeof(return_message), "%s: %ds", myargs[0], elapsed_int);
            }
            else{
                return_message[0] = '\0';
            }

            if(*prev_fd != -1) {
                close(*prev_fd);  // Close the previous command's read end in parent
            }

            if (k < pipe_count - 1 && !output_file ) {
                close(pipefd[1]);  // Close the write end of the pipe in parent
                *prev_fd = pipefd[0];  // Save the read end for the next command
            }
            else if(k < pipe_count - 1 && output_file){
                close(pipefd[1]);  
                close(*fd_null);
                *fd_null = dup(fd_null_0); 
                *prev_fd = *fd_null;
            }
            strcpy(process_info, return_message);
        }                                

    }
}

void execute_my_command_with_pipe(int k, int pipe_count, int* pipefd, char* input_file, char* output_file, int* prev_fd, bool append_mode, int* fd_null, int fd_null_0, char** commands_by_pipe, int* from_history, char* previous_dir, char* home, char** log_commands, int* command_count, char* command, char* process_info, int* process_info_flag){
    if (k < pipe_count - 1) {  // not the last
        pipe(pipefd);
    }

    pid_t pid = fork();

    if (pid == 0) { 
        if(input_file){
            int fd = open(input_file, O_RDONLY);
            if(fd < 0) {
                perror("Error opening input file");
                exit(1);    
            }
            dup2(fd, 0);
            close(fd);  
        }               
        else if(*prev_fd != -1) {  // Not the first command
            dup2(*prev_fd, 0);  // Take input from the previous command
            close(*prev_fd);  // Close the previous command's read end
        }

        if(output_file && k == pipe_count - 1) { // last command has output file
            int fd;
            if (append_mode) fd = open(output_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
            else fd = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

            if(fd < 0) {
                perror("Error opening output file");
                exit(1);
            }
            dup2(fd, 1);
            close(fd);
        }
        else if(output_file && k < pipe_count - 1 ){  // Not the last command but has output file
            int fd;
            if (append_mode) fd = open(output_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
            else fd = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

            if(fd < 0) {
                perror("Error opening output file");
                exit(1);
            }
            dup2(fd, 1);
            close(fd);

            dup2(pipefd[1], *fd_null);
            close(pipefd[0]);                                        
        }
        else if(!output_file && k < pipe_count - 1) {  // not the last command and no output file
            dup2(pipefd[1], 1);  // send output to the next command 
            close(pipefd[0]);  // close the read end of the pipe
        }

        close(pipefd[1]);   
        execute(commands_by_pipe, from_history, previous_dir, home, log_commands, command_count, command, process_info, process_info_flag, k);
        exit(0);
    }

    int status;
    waitpid(pid, &status, WUNTRACED);

    if(*prev_fd != -1) {
        close(*prev_fd);  // close the previous command's read end in parent
    }

    if (k < pipe_count - 1 && !output_file ) {
        close(pipefd[1]);  // close the write end of the pipe in parent
        *prev_fd = dup(pipefd[0]);  // save the read end for the next command
    }
    else if(k < pipe_count - 1 && output_file){
        close(pipefd[1]);  
        close(*fd_null);
        *fd_null = dup(fd_null_0); 
        *prev_fd = dup(*fd_null);
    } 

    return;
}