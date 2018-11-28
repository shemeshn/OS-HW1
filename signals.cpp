// signals.c
// contains signal handler functions
// contains the function/s that set the signal handlers

#include "signals.h"
/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */

//**************************************************************************************
// function name: signal_handler
// Description: handles signals
//**************************************************************************************
Result signal_handler(int signum, int pid, int job_num)
{
    if(kill(pid, signum)==0) {
        //prints success message
        cout << "signal " << strsignal(signum) << " was sent to pid " << pid << endl;
        return SUCCESS;
    }
    else
        return FAILURE;
}

