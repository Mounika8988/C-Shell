// #include "hop.h"
#include "headers.h"


char* hop(char *path, char* home, char* previous_dir) {
    // printf("Previous directory: %s\n", previous_dir);
    static char temp[PATH_MAX];
    getcwd(temp, sizeof(temp));
    char current_dir[PATH_MAX];

    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("getcwd() error");
        return NULL;
    }

    if (path[0] == '~' && path[1] == '\0') {
        path = home;
    } else if(path[0] == '~') {
        char temp_path[PATH_MAX];
        strcpy(temp_path, home);
        strcat(temp_path, path+1);
        strcpy(path, temp_path);        

    }else if (strcmp(path, "-") == 0) {
        if (strlen(previous_dir) == 0) {
            fprintf(stderr, "hop: No previous directory.\n");
            return NULL;
        }
        path = previous_dir;
    }

    if (chdir(path) != 0) {
        perror("hop");
        return NULL;
    }

    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("getcwd() error");
        return NULL;
    }

    printf("Changed directory to: %s\n", current_dir);

    strncpy(previous_dir, current_dir, sizeof(previous_dir) - 1);
    previous_dir[sizeof(previous_dir) - 1] = '\0';

    return temp;
}


void execute_hop(char* token, char* previous_dir, char* home, int* from_history){
    if(*from_history == 1){ 
            *from_history = 0;
    }

    char** hop_destinations = (char**)malloc(sizeof(char*)*MAX_HOPS);
    for(int k = 0; k < MAX_HOPS; k++){
        hop_destinations[k] = (char*)malloc(sizeof(char)*MAX_DIRLEN);
    }

    char path[PATH_MAX];
    token = strtok(NULL, " ");
    if(token == NULL){
        strcpy(previous_dir, hop("~", home, previous_dir));
        // printf("Previous directory: %s\n", previous_dir);
    }
    else{
        while(token != NULL){
            strcpy(path, token);
            char* result = hop(path, home, previous_dir);
            if(result != NULL){
                strcpy(previous_dir, result);
            }
            else{
                break;
            }
            token = strtok(NULL, " ");                           
        }
    }
}
