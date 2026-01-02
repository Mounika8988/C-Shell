#include "headers.h"

void man_page(char* command){
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];

    char *hostname = "man.he.net";
    // char *path = "/?topic=whoami&section=all";
    char path[100];
    snprintf(path, sizeof(path), "/?topic=%s&section=all", command);

    char *html_content = malloc(MAX_CONTENT_SIZE);
    if (html_content == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for HTML content\n");
        exit(1);
    }
    memset(html_content, 0, MAX_CONTENT_SIZE);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        free(html_content);
        exit(1);
    }

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        free(html_content);
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting");
        close(sockfd);
        free(html_content);
        exit(1);
    }

    snprintf(buffer, sizeof(buffer),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             path, hostname);
    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        perror("Error writing to socket");
        close(sockfd);
        free(html_content);
        exit(1);
    }

    size_t total_bytes = 0;
    ssize_t bytes_read;
    while ((bytes_read = read(sockfd, buffer, BUFFER_SIZE - 1)) > 0) {
        if (total_bytes + bytes_read >= MAX_CONTENT_SIZE) {
            fprintf(stderr, "Error: Content too large\n");
            close(sockfd);
            free(html_content);
            exit(1);
        }
        memcpy(html_content + total_bytes, buffer, bytes_read);
        total_bytes += bytes_read;
    }
    close(sockfd);
    parse_and_print_html(html_content);
    free(html_content);
    return;
}

void execute_iMan(char* token, int* from_history){
    if(*from_history == 1){ 
            *from_history = 0;
    }
    token = strtok(NULL, " ");
    printf("command: %s\n", token);
    man_page(token);
}