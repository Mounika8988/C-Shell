#ifndef _HEADERS_H
#define _HEADERS_H

#include "extra.h"
#include "hop.h"
#include "log.h"
#include "mysh.h"
#include "name.h"
#include "proclore.h"
#include "reveal.h"
#include "seek.h"
#include "system.h"
#include "iMan.h"
#include "neonate.h"
#include "signals.h"
#include "parsing.h"

#define STRLEN_MAX 5000
#define MAX_ENTRIES 5000
#define MAX_COMMANDS 100
#define MAX_COMMAND_LENGTH 1000
#define MAX_LOG_COMMANDS 15
#define MAX_HOPS 15
#define MAX_DIRLEN 100
#define PROCESS_INFO_MAX 100
#define PORT 80
#define BUFFER_SIZE 4096
#define MAX_CONTENT_SIZE 1048576  // 1MB
#define MAX_PID 32768

extern int process_count;
extern pid_t foreground_pid;
extern pid_t shell_pgid;    
extern pid_t shell_pid;

#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"
#define BLUE "\033[0;34m"
#define BLUE_BOLD "\033[1;34m"
#define RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

#endif