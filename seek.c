// #include "seek.h"
#include "headers.h"


void truncate_path(char* search_dir, char* full_path){
    char modified_path[PATH_MAX];
    strcpy(modified_path, full_path + strlen(search_dir) - 1);
    modified_path[0] = '.';

    strcpy(full_path, modified_path);
}

int search_directory(const char *target, const char *dir_path, int only_dirs, int only_files, int execute_flag, char* search_dir, char *found_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("opendir");
        return 0;
    }

    struct dirent *entry;
    int found_count = 0;
    struct stat sb;

    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, target, strlen(target)) == 0) {  
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

            if (stat(full_path, &sb) == 0) {
                if ((only_dirs && S_ISDIR(sb.st_mode)) || 
                    (only_files && S_ISREG(sb.st_mode)) || 
                    (!only_dirs && !only_files)) {

                    found_count++;
                    snprintf(found_path, 1024, "%s", full_path);

                    char display_path[1024];
                    strcpy(display_path, full_path);
                    truncate_path(search_dir, display_path);

                    if (S_ISDIR(sb.st_mode)) {
                        printf(BLUE "%s" RESET "\n", display_path);
                    }else{
                        printf(GREEN "%s" RESET "\n", display_path);
                    }
                }
            }
        }

        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char sub_dir[1024];
            snprintf(sub_dir, sizeof(sub_dir), "%s/%s", dir_path, entry->d_name);
            found_count += search_directory(target, sub_dir, only_dirs, only_files, execute_flag, search_dir, found_path);
        }
    }

    closedir(dir);
    return found_count;
}

void execute_seek(char* token, int* from_history){

    if(*from_history == 1){
        *from_history = 0;
    }

    int only_dirs = 0, only_files = 0, execute_flag = 0;
    char *target = NULL, *directory = ".";
    char* flag1 = NULL;
    char* flag2 = NULL;     
    char flags[1024];
    flags[0] = '\0';

    token = strtok(NULL, " ");

    if(token != NULL){
        if(token[0] == '-'){
            flag1 = token;
            token = strtok(NULL, " ");
            if(token != NULL){
                if(token[0] == '-'){
                    flag2 = token;
                    token = strtok(NULL, " ");
                    if(token != NULL){
                        target = token;
                        token = strtok(NULL, " ");
                        if(token != NULL){
                            directory = token; // two flags and target and directory given
                        }
                        else{
                            // two flags, target, no directory given
                        }
                    }
                    else{
                        printf("No target specified\n");  // err: two flags given, no target
                    }
                }else{
                    target = token;   
                    token = strtok(NULL, " ");
                    if(token != NULL){
                        directory = token;  // one flag and target and directory given
                    } 
                    else{
                        // one flag and target, no directory given
                    }
                }                                
            }else{
                printf("No target specified\n");  //err: only one flag given
            }
        }
        else{
            target = token;          
            token = strtok(NULL, " ");
            if(token != NULL){
                directory = token;  // no flags, target and directory given
            }
            else{
                // no flags, only target given
            }
        }
    }
    else{
        printf("No target specified\n");  // err: nothing given
    }

    if(flag1 != NULL){
        strcat(flags, flag1);
    }

    if(flag2 != NULL){
        strcat(flags, flag2);
    }

    for(int k = 0; k < strlen(flags); k++){
        if(flags[k] == 'd'){
            only_dirs = 1;
        }
        else if(flags[k] == 'f'){   
            only_files = 1;
        }
        else if(flags[k] == 'e'){
            execute_flag = 1;
        }
    }

    if(only_dirs && only_files){
        printf("Invalid flags!\n");
    }
    else{        
        // search_directory(target, directory, only_dirs, only_files, execute_flag, directory);
        char found_path[1024] = {0};
        int found_count = search_directory(target, directory, only_dirs, only_files, execute_flag, directory, found_path);
        
        if (found_count == 0) {
            printf("No match found!\n");
        }
        else if (execute_flag) {
            if (found_count == 1) {
                struct stat sb;
                if (stat(found_path, &sb) == 0) {
                    if (S_ISDIR(sb.st_mode)) {
                        if (access(found_path, X_OK) == 0) {
                            chdir(found_path);
                            // printf("%s\n", found_path);
                        } else {
                            printf("Missing permissions for task!\n");
                        }
                    } else if (S_ISREG(sb.st_mode)) {
                        if (access(found_path, R_OK) == 0) {
                            FILE *file = fopen(found_path, "r");
                            if (file) {
                                char ch;
                                while ((ch = fgetc(file)) != EOF)
                                    putchar(ch);
                                fclose(file);
                            }
                        } else {
                            printf("Missing permissions for task!\n");
                        }
                    }
                }
            } else {
                printf("Multiple matches found. The -e flag is only effective when a single file or directory is found.\n");
            }
        }
    }
}
