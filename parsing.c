#include "headers.h"

void get_file_names(char** commands_by_pipe, char** input_file, char** output_file, bool* append_mode, int k){

    char *input_symbol = strchr(commands_by_pipe[k], '<');
    char *output_symbol = strchr(commands_by_pipe[k], '>');

    if(input_symbol && output_symbol){
        if(input_symbol < output_symbol){
            char temp[MAX_COMMAND_LENGTH];
            strcpy(temp, strtok(commands_by_pipe[k], " "));
            char* token = strtok(NULL, " "); // <
            while(token[0] != '<'){
                strcat(temp, " ");
                strcat(temp, token);
                token = strtok(NULL, " ");
            }
            // token = strtok(NULL, " "); // <
            *input_file = strdup(strtok(NULL, " "));
            token = strtok(NULL, " "); // > / >>
            if(strcmp(token, ">>") == 0){
                *append_mode = true;
            }
            *output_file = strdup(strtok(NULL, " "));
            strcpy(commands_by_pipe[k], temp);
        }
        else{
            char temp[MAX_COMMAND_LENGTH];
            strcpy(temp, strtok(commands_by_pipe[k], " "));
            char* token = strtok(NULL, " "); // > / >>
            while(token[0] != '>'){
                strcat(temp, " ");
                strcat(temp, token);
                token = strtok(NULL, " ");
            }
            if(strcmp(token, ">>") == 0){
                *append_mode = true;
            }
            *output_file = strdup(strtok(NULL, " "));
            token = strtok(NULL, " "); // <
            *input_file = strdup(strtok(NULL, " "));
            strcpy(commands_by_pipe[k], temp);
        }
    }
    else if(input_symbol){
        char temp[MAX_COMMAND_LENGTH];
        strcpy(temp, strtok(commands_by_pipe[k], " "));
        char* token = strtok(NULL, " "); // <
        while(token[0] != '<'){
            strcat(temp, " ");
            strcat(temp, token);
            token = strtok(NULL, " ");
        }
        *input_file = strdup(strtok(NULL, " "));
        strcpy(commands_by_pipe[k], temp);
    }
    else if(output_symbol){
            char temp[MAX_COMMAND_LENGTH];
            // strcpy(temp, strtok(commands_by_pipe[i], " "));
            char* token = strtok(commands_by_pipe[k], " ");
            strcpy(temp, token);
            token = strtok(NULL, " ");
            while(token[0] != '>'){
                strcat(temp, " ");
                strcat(temp, token);
                token = strtok(NULL, " ");
            }
            if(strcmp(token, ">>") == 0){
                *append_mode = true;
            }
            
            *output_file = strdup(strtok(NULL, " "));
            strcpy(commands_by_pipe[k], temp);
    }
}

void parse_by_and(char** command_without_spaces, char** commands_by_and, int* and_count, int i){

    int start_index = 0;
    int end_index = 0;
    for(int k=0; k<strlen(command_without_spaces[i]); k++){
        if(command_without_spaces[i][k] == '&'){
            end_index = k  ;
            strncpy(commands_by_and[*and_count], command_without_spaces[i] + start_index, end_index - start_index + 1);
            commands_by_and[*and_count][strlen(commands_by_and[*and_count])] = '\0';
            (*and_count)++;
            start_index = end_index + 1;
        }

        if(k == strlen(command_without_spaces[i]) - 1 && command_without_spaces[i][k] != '&'){
            end_index = k;
            strncpy(commands_by_and[*and_count], command_without_spaces[i] + start_index, end_index - start_index + 1);
            commands_by_and[*and_count][end_index - start_index + 1] = '\0';
            (*and_count)++;
        }
    } 

}
