// signals.c
// contains signal handler functions
// contains the function/s that set the signal handlers

#include "signals.h"
extern Smash smash;
/*******************************************/
//**************************************************************************************
// Name: handler_cntlc
// Synopsis: handle the Control-C
//**************************************************************************************
void ctrlC_handler(int signal)
{
    //checking if there is a job running in fg
    if(smash.fg_job_pid!=0) {
        int pid = smash.fg_job_pid;
        Job fgJob(smash.fg_job_name, pid);
        //sending the signal to the right process
        if(signal_handler(signal, pid, fgJob)==SUCCESS)
            smash.fg_job_pid = 0;
    }
}

//**************************************************************************************
// Name: handler_cntlz
//Synopsis: handle the Control-Z
//**************************************************************************************
void ctrlZ_handler(int signal)
{
    //checking if there is a job running in fg
    if(smash.fg_job_pid!=0){
        int pid = smash.fg_job_pid;
        Job fgJob(smash.fg_job_name, pid);
        //sending the signal to the right process
        if(signal_handler(signal, pid, fgJob)==SUCCESS)
            smash.fg_job_pid = 0;
        //inserting the process to job list
        smash.job_list.push_back(fgJob);
    }
}
//**************************************************************************************
// function name: signal_handler
// Description: handles signals sent from kill command
//**************************************************************************************
using namespace std;
Result signal_handler(int signum, int pid, Job& job)
{
    if(kill(pid, signum)==0) {
        //prints success message
        cout << "signal " << strsignal(signum) << " was sent to pid " << pid << endl;
        //for jobs list accuracy
        if((signum == SIGTSTP)||(signum == SIGSTOP)){
            job.Stop();
            smash.stopped_jobs.push_back(job);
        }
        else if(signum == SIGCONT){
            job.Resume();
            smash.stopped_jobs.remove(job);
        }
        return SUCCESS;
    }
    else
        return FAILURE;
}

