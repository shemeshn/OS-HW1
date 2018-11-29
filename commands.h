#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
//for c++
#include <list>
#include <string>
#include <iostream>
//for checking file status
#include <sys/stat.h>
//for exec command
#include <unistd.h>

#include "Job.h"

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define MAX_CMD_LIST_SIZE 50 // for history command
#define ARG_ERR "Wrong number of arguments"

using std::list;
using std::string;

typedef struct smash_t {
	list<string> cmd_list;
	list<Job> job_list;
} Smash;

int ExeComp(char* lineSize);
int BgCmd(Smash smash, char* lineSize, void* jobs);
int ExeCmd(Smash smash, void* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
#endif

