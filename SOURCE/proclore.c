#include "headers.h"

void get_process_info(pid_t pid) {
    char path[256], buffer[1024];
    char *status;
    char state;
    long vmsize;
    pid_t pgrp, sid, tty_nr;
    
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *stat_file = fopen(path, "r");
    if (stat_file == NULL) {
        perror("Error opening stat file");
        return;
    }

    fscanf(stat_file, "%d %*s %c %*d %d %d %d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*u %ld",
       &pid, &state, &pgrp, &sid, &tty_nr, &vmsize);    

    fclose(stat_file);
    
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
    } else {
        strcpy(buffer, "Unknown");
    }

    if(pid == shell_pid){
        status = "R+";
    }
    else {    
        if (tty_nr == sid) {
            status = (state == 'R' || state == 'S') ? "R+" : (state == 'Z') ? "Z" : "S+";
        } else {
            status = (state == 'R' || state == 'S') ? "R" : (state == 'Z') ? "Z" : "S";
        }
    }
    
    printf("pid : %d\n", pid);
    printf("process status : %s\n", status);
    printf("Process Group : %d\n", pgrp);
    printf("Virtual memory : %ld\n", vmsize);
    printf("executable path : %s\n", buffer);
}

void execute_proclore(char* token, int* from_history){
    if(*from_history == 1){
        *from_history = 0;
    }
    token = strtok(NULL, " ");
    int pid;
    if(token != NULL){
        pid = atoi(token);
    }
    else{
        pid = getpid();
    }
    get_process_info(pid);
}

