#include "headers.h"

#define MAX_PROCESSES 1024

typedef struct {
    pid_t pid;  
    char name[256];
    char status[10];
} ProcessEntry;

ProcessEntry process_table[MAX_PROCESSES];
int process_count = 0;


void add_process(pid_t pid, const char* name) {
    if (process_count < MAX_PROCESSES) {
        process_table[process_count].pid = pid;
        strncpy(process_table[process_count].name, name, sizeof(process_table[process_count].name) - 1);
        strcpy(process_table[process_count].status, "Running");
        process_count++;
    }
}

const char *get_process_name(pid_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            return process_table[i].name;
        }
    }
    return "Unknown";
}

void remove_process(pid_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            for (int j = i; j < process_count - 1; j++) {
                process_table[j] = process_table[j + 1];
            }
            process_count--;
            break;
        }
    }                       
}

void update_process_status(pid_t pid, const char* status) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            strcpy(process_table[i].status, status);
            // printf("updated status to %s\n", status);    
            break;
        }
    }
}

void launch_process(const char *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp(cmd, cmd, NULL);
        exit(1);
    } else {    
        // get the process name from /proc/<pid>/comm
        char path[256];
        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        int fd = open(path, O_RDONLY);
        if (fd != -1) {
            char name[256];
            ssize_t len = read(fd, name, sizeof(name) - 1);
            if (len > 0) {
                name[len] = '\0';
                char *newline = strchr(name, '\n');
                if (newline) {
                    *newline = '\0';
                }
                add_process(pid, name);
            }
            close(fd);
        }
    }
}

void check_process_status(pid_t pid) {
    char path[PATH_MAX], buffer[1024];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE* stat_file = fopen(path, "r");
    if (stat_file == NULL) {
        remove_process(pid);
        return;
    }
    if (fgets(buffer, sizeof(buffer), stat_file) == NULL) {
        perror("fgets");
        update_process_status(pid, "Unknown");
        fclose(stat_file);
        return;
    }
    fclose(stat_file);

    char *token = strtok(buffer, " "); // pid
    token = strtok(NULL, " ");    // command
    token = strtok(NULL, " ");    // status

    if (token != NULL) {
        if (strcmp(token, "R") == 0) {
            update_process_status(pid, "Running");
        } else if (strcmp(token, "S") == 0 || strcmp(token, "D") == 0) {
            // update_process_status(pid, "Sleeping");
            update_process_status(pid, "Running");
        } else if (strcmp(token, "T") == 0) {
            update_process_status(pid, "Stopped");
        } else if (strcmp(token, "Z") == 0) {
            update_process_status(pid, "Zombie");
        } else {    
            update_process_status(pid, "Unknown");
        }
    } else {
        update_process_status(pid, "Error parsing status");
    }
}

void show_activities() {

    for (int i = 0; i < process_count; i++) {
        check_process_status(process_table[i].pid);
    }

    for (int i = 0; i < process_count; i++) {
        printf("%d : %s - %s\n", process_table[i].pid, process_table[i].name, process_table[i].status);
    }   
    
    for (int i = 0; i < process_count; i++) {
        if (strcmp(process_table[i].status, "Exited") == 0 || strcmp(process_table[i].status, "Terminated") == 0) {
            for (int j = i; j < process_count - 1; j++) {
                process_table[j] = process_table[j + 1];
            }
            process_count--;
            i--; 
        }
    }   
}

void ping_process(pid_t pid, int signal_number) {
    signal_number %= 32;
    int result = kill(pid, signal_number);
    
    if (result == 0) {
        printf("Sent signal %d to process with pid %d\n", signal_number, pid);
        if (signal_number == SIGKILL || signal_number == SIGTERM) {
            update_process_status(pid, "Terminated");
        } else if (signal_number == SIGSTOP) {
            update_process_status(pid, "Stopped");
        }
    } else {
        printf("No such process found\n");
    }
}

void system_command_execute(char* command, char* process_info) {

    char* myargs[4096];
    char return_message[4096];  
    int is_background = 0;

    if (command[strlen(command) - 1] == '&') {
        is_background = 1;
        // printf("Background process\n");
        command[strlen(command) - 1] = '\0';  // Remove '&' from the command
    }       
    
    char* token = strtok(command, " ");
    int i = 0;  
    int in_quotes = 0;
    char* arg = NULL;    

    while (token != NULL) {
        if (token[0] == '"') {
            in_quotes = 1;
            arg = strdup(token + 1);
            if (strchr(arg, '"')) {
                arg[strlen(arg) - 1] = '\0';
                myargs[i++] = arg;
                in_quotes = 0;
            }
        } else if (in_quotes) {
            if (strchr(token, '"')) {
                token[strlen(token) - 1] = '\0'; 
                arg = realloc(arg, strlen(arg) + strlen(token) + 1);
                strcat(arg, " ");
                strcat(arg, token);
                myargs[i++] = arg;
                in_quotes = 0;
            } else {
                arg = realloc(arg, strlen(arg) + strlen(token) + 2);
                strcat(arg, " ");
                strcat(arg, token);
            }
        } else {
            myargs[i++] = strdup(token);
        }
        token = strtok(NULL, " ");
    }
    myargs[i] = NULL;

    time_t start, end;
    double elapsed;
    int elapsed_int;    
    start = time(NULL);

    pid_t rc = fork();

    if(rc == 0){
        if(execvp(myargs[0], myargs)==-1){
            perror("Invalid command");
            exit(1);
        }
    }
    else if(rc > 0){
        if(is_background == 1){
            add_process(rc, myargs[0]);
            printf("%d\n", rc);  // print pic of background process
        }       
        else{
            add_process(rc, myargs[0]);
            foreground_pid = rc;
            int status;
            waitpid(rc, &status, WUNTRACED);  // wait for child process to finish
            foreground_pid = -1;
            if (WIFSTOPPED(status)) {   
                update_process_status(rc, "Stopped");
            }
            else if(WIFSIGNALED(status) && WTERMSIG(status) == SIGINT){
                printf("\n");
            } else {
                remove_process(rc);
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
            strcpy(process_info, return_message);  
        }     
    }
    else{
        perror("fork failed");              
    }

    return;
}


void execute_ping(char* token, int* from_history){
    if(*from_history == 1){
        *from_history = 0;
    }
    token = strtok(NULL, " ");
    if(token == NULL){
        printf("Usage: ping <pid> <signal_number>\n");
        return;
    }

    pid_t pid = atoi(token);
    printf("pid: %d\n", pid);

    token = strtok(NULL, " ");
    if(token == NULL){
        printf("Usage: ping <pid> <signal_number>\n");
        return;
    }

    int signal_number = atoi(token);
    // printf("signal_number: %d\n", signal_number);
    ping_process(pid, signal_number);   
    return; 
}

void execute_activities(int* from_history){
    // printf("going into execute_activities\n");
    if(*from_history == 1){
        *from_history = 0;
    }
    show_activities();
    return;
}


void resume_bg(pid_t pid) {
    // Resume the stopped process in the background
    if (kill(pid, SIGCONT) == 0) {
        printf("Process (PID %d) resumed in background.\n", pid);
        update_process_status(pid, "Running");
    } else {
        perror("Error sending SIGCONT to process");
    }
}

void execute_bg(char* token, int* from_history){
    if(*from_history == 1){
        *from_history = 0;
    }
    token = strtok(NULL, " ");
    if(token == NULL){
        printf("Usage: bg <pid>\n");
        return;
    }
    pid_t pid = atoi(token);
    resume_bg(pid);
    return;     
}

void bring_to_foreground(pid_t pid) {
    pid_t pg = getpgid(pid);
    // ensure the process exists and belongs to the foreground process group
    if (tcsetpgrp(STDIN_FILENO, pg) == -1) {
        perror("Error setting foreground process group");
        return;
    }

    if (kill(pid, SIGCONT) == 0) {
        printf("Process (PID %d) brought to foreground.\n", pid);
    } else {
        perror("Error sending SIGCONT to process");
        return;
    }

    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
        perror("Error waiting for process");
        return;
    }

    if (tcsetpgrp(STDIN_FILENO, shell_pgid) == -1) {
        perror("Error restoring shell process group");
    }   
}

void execute_fg(char* token, int* from_history) {
    if (*from_history == 1) {
        *from_history = 0;
    }

    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("Usage: fg <pid>\n");
        return;
    }

    pid_t pid = atoi(token);
    bring_to_foreground(pid);
    return;    
}