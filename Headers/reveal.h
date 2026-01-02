#ifndef _REVEAL_H
#define _REVEAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <linux/limits.h>

void execute_reveal(char* token, int* from_history, char* previous_dir, char* home);


#endif