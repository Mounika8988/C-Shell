#ifndef _SIGNALS_H_
#define _SIGNALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>


void handle_sigint(int sig);
void handle_sigtstp(int sig);
void handle_sigchld(int sig); 

#endif