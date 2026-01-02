#ifndef _PROCLORE_H_
#define _PROCLORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

void execute_proclore(char* token, int* from_history);

#endif