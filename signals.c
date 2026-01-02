#include "headers.h"

int foreground_pid = -1;

void handle_sigint(int sig) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
        remove_process(foreground_pid);
    } else {
        printf("\n");   
    }
}

void handle_sigtstp(int sig) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGTSTP);
        update_process_status(foreground_pid, "Stopped");
        foreground_pid = -1;
    }
    else{
        printf("\nNo foreground process to stop (except shell).\n");
    }
}

void handle_sigchld(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        const char *name = get_process_name(pid);

        if (WIFEXITED(status)) {
            printf("\"%s\" exited normally (%d)\n", name, pid);
            update_process_status(pid, "Exited");
        } else if (WIFSIGNALED(status)) {
            printf("\"%s\" (%d) was terminated by a signal\n", name, pid);
            update_process_status(pid, "Terminated");
        }
        remove_process(pid);
    }
}
