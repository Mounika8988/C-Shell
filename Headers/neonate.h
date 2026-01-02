#ifndef _NEONATE_H_
#define _NEONATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <time.h>

void execute_neonate(char* token, int* from_history);

#endif
