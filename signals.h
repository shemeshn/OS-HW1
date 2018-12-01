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
#include <list>
#include <string>

#include "Job.h"
using namespace std;

typedef struct smash_t {
    int fg_job_pid;
    string fg_job_name;
    list<string> cmd_list;
    list<Job> job_list;
    list<Job> stopped_jobs;
} Smash;

typedef enum {SUCCESS, FAILURE} Result;

void ctrlC_handler(int signal);
void ctrlZ_handler(int signal);
Result signal_handler(int signum, int pid, Job& job);

#endif

