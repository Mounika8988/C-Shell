#include "headers.h"

void list_directory(const char *path, int show_all, int show_details) {
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("opendir");
        return;
    }
    char *entries[MAX_ENTRIES];
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (show_all || entry->d_name[0] != '.') {
            if (count < MAX_ENTRIES) {          
                entries[count] = strdup(entry->d_name);
                count++;
            }
        }
    }
    closedir(dir);    
    qsort(entries, count, sizeof(char *), compare_strings);

    for (int i = 0; i < count; i++) {
        struct stat statbuf;
        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "%s/%s", path, entries[i]);
        if (stat(filepath, &statbuf) == 0) {

            if (S_ISDIR(statbuf.st_mode)) printf(BLUE); //directory - blue
            else if (statbuf.st_mode & S_IXUSR) printf(GREEN);  //executable - green
            else printf(RESET);  //regular file - default white               

            if (show_details) {
                printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-");
                printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
                printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
                printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
                printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
                printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
                printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
                printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
                printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
                printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");
                printf(" %ld", (long)statbuf.st_nlink);
                
                struct passwd *pw = getpwuid(statbuf.st_uid);
                struct group *gr = getgrgid(statbuf.st_gid);
                printf(" %s %s", pw ? pw->pw_name : "UNKNOWN", gr ? gr->gr_name : "UNKNOWN");
                
                printf(" %ld", (long)statbuf.st_size);
                
                //modification time
                char timebuf[64];
                struct tm *tm_info = localtime(&statbuf.st_mtime);
                strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
                printf(" %s", timebuf);
            }

            printf(" %s\n", entries[i]);
            printf(RESET); //reset color  
        } else {
            perror("stat");
        }
        free(entries[i]);
    }
}


void reveal(const char *flags, const char *path, char* previous_dir) {
    int show_all = 0;
    int show_details = 0;

    if (strchr(flags, 'a')) show_all = 1;
    if (strchr(flags, 'l')) show_details = 1;

    char resolved_path[PATH_MAX];
    if (strcmp(path, ".") == 0) {
        getcwd(resolved_path, sizeof(resolved_path));
    } else if (strcmp(path, "..") == 0) {
        if (getcwd(resolved_path, sizeof(resolved_path)) != NULL) {
            char *last_slash = strrchr(resolved_path, '/');
            if (last_slash != NULL && last_slash != resolved_path) {
                *last_slash = '\0';
            }
        }
    } else if (strcmp(path, "-") == 0) {
        if(strlen(previous_dir) == 0) {
            printf("No previous directory\n");
            return;
        }
        strcpy(resolved_path, previous_dir);
    } else {
        realpath(path, resolved_path);
    }

    list_directory(resolved_path, show_all, show_details);
}

void execute_reveal(char* token, int* from_history, char* previous_dir, char* home) {
    if(*from_history == 1){
        *from_history = 0;
    }
    char *flag1 = NULL;

    char path[PATH_MAX];
    char *flag2 = NULL;

    char flags[MAX_COMMANDS];
    flags[0] = '-';
    flags[1] = '\0';

    token = strtok(NULL, " ");

    if(token != NULL){
        if(token[0] == '-'){  // if the first argument is a flag
            if(strlen(token) == 1){
                strcpy(path, "-");
            }
            else{
                flag1 = token;
                token = strtok(NULL, " ");
                if(token != NULL){
                    if(token[0] == '-'){  // if the second argument is a flag
                        if(strlen(token) == 1){
                            strcpy(path, "-");
                        }
                        else{
                            flag2 = token;
                            printf("flag2: %s\n", token);
                            token = strtok(NULL, " ");
                            if(token != NULL){
                                strcpy(path, token);  // if there is a path specified and two flags are given
                            }
                            else{
                                strcpy(path, ".");  // if there is no path specified and two flags are given
                            }
                        }
                    }else{
                        strcpy(path, token);   // one flag and path
                    }
                }else{
                    strcpy(path, ".");  // if there is no path specified and one flag is given                            
                }
            }
        }
        else{
            strcpy(path, token);  // if there is no flag specified and path is given
        }
    }
    else{
        strcpy(path, "."); // if there is no flag and path specified
        strcpy(flags, "");                 
    }                

    if(flag1 != NULL){
        flag1++;
        // printf("length of flag1: %ld\n", strlen(flag1));
        strcat(flags, flag1);
    }
    if(flag2 != NULL){
        flag2++;
        // printf("length of flag2: %ld\n", strlen(flag2));
        strcat(flags, flag2);
    }

    char *ptr = path;
    char temp[PATH_MAX];
    strcpy(temp, home);

    if(path[0] == '~' && path[1] != '0'){
        ptr++;
        strcat(home, ptr);
        strcpy(ptr, home);
        strcpy(home, temp);            
    }
    else if(path[0] == '~'){
        strcpy(path, home);
    }

    reveal(flags, ptr, previous_dir);
}
