#ifndef _SEEK_H_
#define _SEEK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>

void execute_seek(char* token, int* from_history);

#endif