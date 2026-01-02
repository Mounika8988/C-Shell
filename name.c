#include "headers.h"

char* getusername(){

    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL){
        perror("getpwuid() error");
        return NULL;
    }

    char* username = (char*)malloc(sizeof(char)*STRLEN_MAX);
    strncpy(username, pw->pw_name, STRLEN_MAX-1);
    username[STRLEN_MAX-1] = '\0';

    char hostname[STRLEN_MAX]; 
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        free(username); 
        return NULL; 
    }

    strcat(username, "@"); 
    strcat(username, hostname);
    return username;    
}

