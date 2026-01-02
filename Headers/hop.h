#ifndef _HOP_H_
#define _HOP_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

void execute_hop(char* token, char* previous_dir, char* home, int* from_history);

#endif