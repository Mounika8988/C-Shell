#include "headers.h"


int has_substring(char *string, char *substring) {
    if (strstr(string, substring) != NULL) {
        return 1;
    }
    return 0;
}

void trim_and_remove_extra_spaces(char *input, char *output) {
    char *in = input;
    char *out = output;

    while (isspace(*in)) {
        in++;
    }
    int space_flag = 0; 
    while (*in != '\0') {
        if (!isspace(*in)) {
            *out = *in;
            out++;
            in++;
            space_flag = 0;
        } else {
            if (!space_flag) {
                *out = ' ';
                out++;
                space_flag = 1;
            }
            while (isspace(*in)) {
                in++;
            }
        }
    }

    if(*(out-1) == ' '){
        out--;
    }    
    *out = '\0';
}

char* relative_path(char* home, char* path){
    static char temp[PATH_MAX];

    if(strlen(home) > strlen(path)){
        strcpy(temp, path);
        return temp;
    }
    else{
        int i = 0;
        temp[0] = '~';
        while(home[i] == path[i] && home[i] != '\0'){
            i++;
        }
        int j = 1;
        while(i < strlen(path)){
            temp[j] = path[i];
            i++; j++;
        }
        temp[j] = '\0';
    }
    return temp;
}


void process_name(int pid, char* pname) {
    char path[256];
    char process_name[256];
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);

    FILE* fptr = fopen(path, "r");
    if (fptr == NULL) {
        perror("fopen");
        strcpy(pname, "Unknown"); // Set a default name in case of error
        return;
    }

    if (fgets(process_name, sizeof(process_name), fptr) != NULL) {
        int len = strlen(process_name);
        if (len > 0 && process_name[len - 1] == '\n') {
            process_name[len - 1] = '\0';
        }
        strcpy(pname, process_name);
    } else {
        strcpy(pname, "Unknown"); // Set a default name in case of error
    }

    fclose(fptr);
}


void replace_start(char *A, const char *B, const char *C) {
    if (strncmp(A, B, strlen(B)) == 0) {
        char temp[STRLEN_MAX];

        strcpy(temp, C);
        strcat(temp, A + strlen(B));
        strcpy(A, temp);
    }
}


void add_spaces(char *input, char *output) {
    int i = 0, j = 0;
    while (input[i] != '\0') {
        // Check for '>>'
        if (input[i] == '>' && input[i + 1] == '>') {
            if (j > 0 && output[j - 1] != ' ') {
                output[j++] = ' ';
            }
            output[j++] = '>';
            output[j++] = '>';
            if (input[i + 2] != ' ') {
                output[j++] = ' ';
            }
            i += 2;
        }
        // Check for '>'
        else if (input[i] == '>') {
            if (j > 0 && output[j - 1] != ' ') {
                output[j++] = ' ';
            }
            output[j++] = '>';
            if (input[i + 1] != ' ') {
                output[j++] = ' ';
            }
            i++;
        }
        // Check for '<'
        else if (input[i] == '<') {
            if (j > 0 && output[j - 1] != ' ') {
                output[j++] = ' ';
            }
            output[j++] = '<';
            if (input[i + 1] != ' ') {
                output[j++] = ' ';
            }
            i++;
        }
        // Copy regular characters
        else {
            output[j++] = input[i++];
        }
    }
    output[j] = '\0';  // Null terminate the output string
}


void redirect_input(char* input_file) {
    if(input_file == NULL) {
        return;
    }
    int fd = open(input_file, O_RDONLY);
    if(fd < 0) {
        perror("Error opening input file");
        exit(1);
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
}

void redirect_output(char* output_file, bool append_mode) {
    if(output_file == NULL) {
        return;
    }
    int fd;
    if (append_mode) 
        fd = open(output_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
    else 
        fd = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

    if(fd < 0) {
        perror("Error opening output file");
        exit(1);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
}

void parse_and_print_html(char *html_content) {
    char *body_start = strstr(html_content, "<body");
    if (body_start == NULL) {
        body_start = html_content;
    } else {
        body_start = strchr(body_start, '>');
        if (body_start) body_start++;
    }

    int in_tag = 0;
    int new_line = 1;
    int consecutive_newlines = 0;
    char *p = body_start;

    while (*p) {
        if (*p == '<') {
            in_tag = 1;
            if (strncmp(p, "</p>", 4) == 0 || strncmp(p, "</div>", 6) == 0 || 
                strncmp(p, "</h1>", 5) == 0 || strncmp(p, "</h2>", 5) == 0 ||
                strncmp(p, "</h3>", 5) == 0 || strncmp(p, "</h4>", 5) == 0 ||
                strncmp(p, "</h5>", 5) == 0 || strncmp(p, "</h6>", 5) == 0 ||
                strncmp(p, "<br", 3) == 0 || strncmp(p, "<hr", 3) == 0) {

                printf("\n");
                // new_line = 1;
                if (consecutive_newlines < 2) {
                    printf("\n");
                    consecutive_newlines++;
                }
                new_line = 1;
            }
        } else if (*p == '>') {
            in_tag = 0;
        } else if (!in_tag) {
            if (new_line && isspace(*p)) {
                // skip leading whitespace on new lines
                p++;
                continue;
            }
            putchar(*p);
            new_line = 0;
            consecutive_newlines = 0;
        }
        p++;    
    }
}

void initialize_file_names(char** commands_by_pipe, char** input_file, char** output_file, bool* append_mode, int k) {
    trim_and_remove_extra_spaces(commands_by_pipe[k], commands_by_pipe[k]);
    char temp[MAX_COMMAND_LENGTH];
    strcpy(temp, commands_by_pipe[k]);
    add_spaces(temp, commands_by_pipe[k]);
    get_file_names(commands_by_pipe, input_file, output_file, append_mode, k);

    char temp_command[MAX_COMMAND_LENGTH];
    temp_command[0] = '\0';
    trim_and_remove_extra_spaces(commands_by_pipe[0], temp_command);
    strcpy(commands_by_pipe[0], temp_command);
    if(*input_file){              
        temp_command[0] = '\0';  
        trim_and_remove_extra_spaces(*input_file, temp_command);
        strcpy(*input_file, temp_command);
    }
    if(*output_file){
        temp_command[0] = '\0';
        trim_and_remove_extra_spaces(*output_file, temp_command);
        strcpy(*output_file, temp_command); 
    }
}

void free_array(char** array, int size) {
    for (int i = 0; i < size; i++) {
        free(array[i]);
    }
    free(array);
}

int compare_strings(const void *a, const void *b) {
    return strcasecmp(*(const char **)a, *(const char **)b);
}





