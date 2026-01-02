#include "headers.h"

pid_t find_newest_pid() {   
    DIR *dir;
    struct dirent *entry;
    pid_t max_pid = 0;
    time_t max_time = 0;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Error opening /proc");
        exit(1);
    }       

    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            pid_t pid = atoi(entry->d_name);
            char path[256];
            struct stat st;

            snprintf(path, sizeof(path), "/proc/%d", pid);
            if (stat(path, &st) == 0) {
                if (st.st_ctime > max_time) {
                    max_time = st.st_ctime;
                    max_pid = pid;
                }
            }
        }
    }

    closedir(dir);
    return max_pid;
}

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void execute_neonate(char* token, int* from_history){
    int time_arg;
    token = strtok(NULL, " ");
    if(token){
        if(strcmp(token, "-n") == 0){
            token = strtok(NULL, " ");
            if(token){
                time_arg = atoi(token);
                if(time_arg <= 0){
                    fprintf(stderr, "Invalid time argument. Please provide a positive integer.\n");
                    return;
            }       
            }
            else{
                fprintf(stderr, "Invalid time argument. Please provide a positive integer.\n");
                return;
            }
        }
        else{
            fprintf(stderr, "Invalid flag. Please provide -n flag.\n");
            return;
        }
    }
    else{
        fprintf(stderr, "No flag. Please provide -n flag.\n");
        return;
    }

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (1) {
        pid_t newest_pid = find_newest_pid();
        printf("%d\n", newest_pid);
        fflush(stdout);

        time_t start_time = time(NULL); 
        while (time(NULL) - start_time < time_arg) {
            if (kbhit()) {
                char c = getchar();
                if (c == 'x') {
                    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                    return;
                }
            }
            usleep(100000);  // sleep  100ms to reduce CPU usage
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

}