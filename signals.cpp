// signals.c
// contains signal handler functions
// contains the function/s that set the signal handlers

#include "signals.h"

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */

/* Name: handler_cntlz
   Synopsis: handle the Control-Z */
void ctrlz_handler(Smash& smash)
{

}
//**************************************************************************************
// function name: signal_handler
// Description: handles signals
//**************************************************************************************
using namespace std;
Result signal_handler(int signum, int pid, Job& job)
{
    if(kill(pid, signum)==0) {
        //prints success message
        cout << "signal " << strsignal(signum) << " was sent to pid " << pid << endl;
        //for jobs list accuracy
        if((signum == SIGTSTP)||(signum == SIGSTOP))
            job.Stop();
        else if(signum == SIGCONT)
            job.Resume();
        return SUCCESS;
    }
    else
        return FAILURE;
}

