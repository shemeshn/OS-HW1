#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <iostream>

#include "Job.h"

//useful signal names
//#define SIGINT  2
//#define SIGKILL 9
//#define SIGTERM 15
//#define SIGTSTP 20
//#define SIGCONT 25


typedef enum {SUCCESS, FAILURE} Result;

Result signal_handler(int signum, int pid);

#endif

