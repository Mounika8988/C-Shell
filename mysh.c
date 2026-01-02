#include "headers.h"

char* find_alias(char* temp_token, char* home){

    char temp[PATH_MAX];
    strcpy(temp, home);
    strcat(temp, "/myshrc.txt");

    FILE* fptr = fopen(temp, "r");
    if(fptr == NULL){
        printf("Error opening file\n");
        return NULL;
    }

    char line[MAX_COMMAND_LENGTH];
    char command[MAX_COMMAND_LENGTH];
    command[0] = '\0';

    bool found = false;
    while(1){

        char* ret = fgets(line, sizeof(line), fptr);
        if(ret == NULL ){
            break;  
        }
        int len = strlen(line);
        if(len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        char* saveptr;
        char* token = strtok_r(line, " ", &saveptr);
        if(token != NULL){
            if(strcmp(token, "alias") == 0){
                token = strtok_r(NULL, " ", &saveptr);  
            }

            if(strcmp(token, temp_token) == 0){
                // printf("Command found\n");
                token = strtok_r(NULL, " ", &saveptr);
                if(token != NULL){
                    if(token[0] == '=' && token[1] == '\0'){
                        token = strtok_r(NULL, " ", &saveptr);
                        if(token != NULL){
                            strcpy(command, token);
                            // printf("alias of [%s] is [%s]\n", temp_token, command);
                            found = true;
                        }else{
                            printf("invalid syntax in .myshrc\n");  // = is there but no alias
                        }   
                    }else{
                        printf("invalid syntax in .myshrc\n"); // no = after command
                    }
                }else{
                    printf("invalid syntax in .myshrc\n"); // nothing is there after the word alias.
                }
            }
        }

        if(found){
            break;
        }

    }

    fclose(fptr);

    if(!found){     
        return NULL;
    }   
    char* result = (char*)malloc(sizeof(char)*STRLEN_MAX);

    if (command[0] != '\0'){
        strcpy(result, command);
        return result;
    }

    free(result);
    return NULL;
}
