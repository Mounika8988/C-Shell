#ifndef _IMAN_H_
#define _IMAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>  

void execute_iMan(char* token, int* from_history);

#endif