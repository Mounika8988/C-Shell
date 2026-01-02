// #include "log.h"
#include "headers.h"

char** initialize_log_commands(char* home, int *command_count){

    char** log_commands = (char**)malloc(sizeof(char*)*MAX_LOG_COMMANDS);
    for(int i = 0; i < MAX_LOG_COMMANDS; i++){
        log_commands[i] = (char*)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
    }

    char temp[PATH_MAX];
    strcpy(temp, home);
    strcat(temp, "/log.txt");

    FILE *fp = fopen(temp, "r");
    if (fp == NULL) {
        perror("Unable to open file");
        return NULL;
    }

    while (fgets(log_commands[*command_count], MAX_COMMAND_LENGTH, fp) != NULL) {
        log_commands[*command_count][strcspn(log_commands[*command_count], "\n")] = '\0';
        *command_count = *command_count + 1;
    }

    fclose(fp);

    return log_commands;

}


void add_command(char** log_commands, char* command, int *command_count){

    if( *command_count > 0 &&  strcmp(log_commands[*command_count - 1], command) == 0){
        // printf("same command as the previous one, not logged\n");
        return;
    }

    if(*command_count >= MAX_LOG_COMMANDS){
        for(int i = 0; i < MAX_LOG_COMMANDS - 1; i++){
            strcpy(log_commands[i], log_commands[i+1]);
            // printf("moved command in line %d to line %d\n", i+2, i+1);
        }
        strcpy(log_commands[MAX_LOG_COMMANDS - 1], command);
        // printf("added command to line 15\n");
    }
    else{
        strcpy(log_commands[*command_count], command);
        *command_count = *command_count + 1;
        // printf("added command to line %d\n", *command_count+1);
        return;
    }
}

char* return_command(char** log_commands, int index, int command_count){
    if(index > command_count){
        return NULL;
    }
    return log_commands[command_count - index];    
}

void update_log(char** log_commands, char* home, int command_count){
    char temp[PATH_MAX];
    strcpy(temp, home);
    strcat(temp, "/log.txt");

    FILE *fp = fopen(temp, "w");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }

    for(int i = 0; i < command_count; i++){
        if(log_commands[i][0] == '\0'){
            break;
        }
        fprintf(fp, "%s\n", log_commands[i]);
    }
    fclose(fp);  
}

void purge_log_file(char* home){
    char temp[PATH_MAX];
    strcpy(temp, home);
    strcat(temp, "/log.txt");

    FILE *fp = fopen(temp, "w");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }
    fclose(fp);
}

void print_log(char** log_commands, int command_count){
    for(int i = 0; i < command_count; i++){
        if(log_commands[i][0] == '\0'){
            break;
        }
        printf("%s\n", log_commands[i]);
    }
}

void purge_log(char** log_commands, int *command_count){
    for(int i = 0; i < MAX_LOG_COMMANDS; i++){
        log_commands[i][0] = '\0';
    }
    *command_count = 0;
}

void execute_log(char* token, char** log_commands, int* command_count, char* home, char* command, int* from_history){
    token = strtok(NULL, " ");
    if(token != NULL){
        if(strcmp(token, "purge") == 0){
            if(strtok(NULL, " ") != NULL){
                printf("invalid log command\n");
                return;
            }
            purge_log(log_commands, command_count);
            purge_log_file(home);
        }
        else if(strcmp(token, "execute") == 0){
            char* token = strtok(NULL, " ");
            if(token != NULL){
                if(strtok(NULL, " ") != NULL){
                    printf("invalid log command\n");
                    return;
                }                
                int index = atoi(token);
                if(index > *command_count || index <= 0){
                    printf("invalid command index\n");
                }
                else{
                    char* retrieved_command = return_command(log_commands, index, *command_count);
                    if(retrieved_command == NULL){
                        perror("log");
                    }
                    else{
                        // printf("retrieved command: %s\n", retrieved_command);
                        strcpy(command, retrieved_command);
                        *from_history = 1;
                    }
                }
            }
            else{
                printf("command index not specified\n");
            }
        }
        else{
            printf("invalid log command\n");
        }
    }
    else{
        print_log(log_commands, *command_count);  // nothing specified, so just show history
    }
}
