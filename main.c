#include "headers.h"
#include "execute.h"

pid_t shell_pgid;
pid_t shell_pid;    

int main(){ 

    shell_pid = getpid();
    shell_pgid = getpgid(shell_pid); 
    char* username = getusername();
    char home[PATH_MAX];
    getcwd(home, sizeof(home));

    char previous_dir[PATH_MAX];
    previous_dir[0]  = '\0';

    char process_info[PROCESS_INFO_MAX];                
    process_info[0] = '\0';   
    char command[MAX_COMMAND_LENGTH]; 

    int command_count = 0;
    int process_info_flag = 0;
    int from_history = 0;   
    
    struct sigaction sa;
    
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    signal(SIGCHLD, handle_sigchld);
    signal(SIGINT, handle_sigint); 
    signal(SIGTSTP, handle_sigtstp);    

    char **log_commands = initialize_log_commands(home, &command_count);

    char** command_without_spaces = (char**)malloc(sizeof(char*)*MAX_COMMANDS);
    for(int i = 0; i < MAX_COMMANDS; i++){
        command_without_spaces[i] = (char*)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
    }

    while(1){
        char current_dir[PATH_MAX];
        getcwd(current_dir, sizeof(current_dir));
        strcpy(current_dir, relative_path(home, current_dir));

        if(from_history == 0){

            if (process_info_flag == 1) {
                process_info_flag = 0;
                printf(
                    YELLOW "<" RESET        // Yellow '<'
                    CYAN "%s" RESET         // Cyan username
                    YELLOW ":" RESET        // Yellow ':'
                    BLUE "%s" RESET         // Blue current_dir
                    YELLOW " " RESET        // Yellow space
                    RED "%s" RESET          // Red process_info
                    YELLOW ">" RESET " ",   // Yellow '>'
                    username, current_dir, process_info
                );
            } else {
                printf(
                    YELLOW "<" RESET        // Yellow '<'
                    CYAN "%s" RESET         // Cyan username
                    YELLOW ":" RESET        // Yellow ':'
                    BLUE "%s" RESET         // Blue current_dir
                    YELLOW ">" RESET " ",   // Yellow '>'
                    username, current_dir
                );
            }

            if(fgets(command, sizeof(command), stdin) == NULL){
                if(feof(stdin)){
                    printf("\nLogging out...\n");
                    break;
                }else{
                    perror("fgets");
                    exit(1);
                }
            }

            if(command[0] == '\n') continue;
            command[strlen(command)-1] = '\0';
                
            if(!has_substring(command, "log")){
                add_command(log_commands, command, &command_count);
            }
            update_log(log_commands, home, command_count);
        }
        

        int number_of_commands = 0;
        char* token = strtok(command, ";");
        while(token != NULL){   
            trim_and_remove_extra_spaces(token, command_without_spaces[number_of_commands]);
            number_of_commands++;
            token = strtok(NULL, ";");
        }

        for(int i = 0; i < number_of_commands; i++){

            char **commands_by_and = (char**)malloc(sizeof(char*)*MAX_COMMANDS);
            for(int k = 0; k < MAX_COMMANDS; k++){
                commands_by_and[k] = (char*)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
            }   

            int and_count = 0;
            parse_by_and(command_without_spaces, commands_by_and, &and_count, i);

            for(int j = 0; j < and_count; j++){

                if(commands_by_and[j][strlen(commands_by_and[j]) - 1] == '|' || commands_by_and[j][0] == '|'){
                    if(from_history){
                        from_history = 0;
                    }
                    printf("syntax error for '|'\n");   
                    continue;
                }

                char **commands_by_pipe = (char**)malloc(sizeof(char*)*MAX_COMMANDS);  
                int pipe_count = 0;
                char *token = strtok(commands_by_and[j], "|");
                while(token != NULL) {
                    commands_by_pipe[pipe_count] = strdup(token);  // Allocate memory and copy the command
                    pipe_count++;
                    token = strtok(NULL, "|");
                }


                if(pipe_count == 1){  // no pipes                       
                    bool append_mode = false;
                    char* input_file = NULL;
                    char* output_file = NULL;
                    initialize_file_names(commands_by_pipe, &input_file, &output_file, &append_mode, 0);

                    // printf("command: %s\n", commands_by_pipe[0]);
                    // printf("input file: %s\n", input_file);
                    // printf("output file: %s\n", output_file);                    

                    redirect_input(input_file);
                    redirect_output(output_file, append_mode);

                    execute(commands_by_pipe, &from_history, previous_dir, home, log_commands, &command_count, command, process_info, &process_info_flag, 0);

                    redirect_input("/dev/tty");
                    redirect_output("/dev/tty", false);
                }
                else{
                    int prev_fd = -1;

                    int fd_null = open("/dev/null", O_RDONLY);
                    if(fd_null < 0){
                        perror("Error opening /dev/null");
                        exit(1);
                    }
                    int fd_null_0 = dup(fd_null);   

                    for(int k = 0; k < pipe_count; k++) {
                        int pipefd[2];
                        bool append_mode = false;
                        char* input_file = NULL;
                        char* output_file = NULL;

                        initialize_file_names(commands_by_pipe, &input_file, &output_file, &append_mode, k);

                        if(isSystemCommand(commands_by_pipe[k], home)){
                            // printf("[%s] is a syscommand!\n", commands_by_pipe[k]);
                            execute_syscommand_with_pipe(k, pipe_count, pipefd, commands_by_pipe, input_file, output_file, append_mode, &prev_fd, &fd_null, fd_null_0, process_info);
                            redirect_input("/dev/tty");
                            redirect_output("/dev/tty", false);
                            if(process_info[0] != '\0'){
                                process_info_flag = 1;
                            }                           
                        }

                        else{
                            // printf("[%s] is not a syscommand!\n", commands_by_pipe[k]);
                            execute_my_command_with_pipe(k, pipe_count, pipefd, input_file, output_file, &prev_fd, append_mode, &fd_null, fd_null_0, commands_by_pipe, &from_history, previous_dir, home, log_commands, &command_count, command, process_info, &process_info_flag);
                            redirect_input("/dev/tty");
                            redirect_output("/dev/tty", false);   
                            if(process_info[0] != '\0'){
                                process_info_flag = 1;  
                            }        
                        }   

                    }                
                    close(fd_null);
                    close(fd_null_0);
                }
                free_array(commands_by_pipe, pipe_count);
            }
            free_array(commands_by_and, MAX_COMMANDS);
        }        
    }
    free_array(command_without_spaces, MAX_COMMANDS);
    free_array(log_commands, MAX_LOG_COMMANDS);

    free(username);
    return 0;
}

