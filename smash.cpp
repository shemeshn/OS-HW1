/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

using namespace std;
char* L_Fg_Cmd;
char lineSize[MAX_LINE_SIZE];

// Global variables
Smash smash;

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdArr[MAX_LINE_SIZE]; 	   
    string cmdString;
	smash.fg_job_pid = 0;
	//signal declarations
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */
	/************************************/
	signal(SIGTSTP, ctrlZ_handler);
	signal(SIGINT, ctrlC_handler);
	/************************************/
	// Init globals 
    
	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		UpdateJobsList(smash);
		strcpy(cmdArr, lineSize);    	
		cmdArr[strlen(lineSize)-1]='\0';
		cmdString = cmdArr;
        //saving new commands to command list for history command
        if(!cmdString.empty()){
            if (smash.cmd_list.size()>=MAX_CMD_LIST_SIZE)
                smash.cmd_list.pop_front();
            smash.cmd_list.push_back(cmdString);
        }
					// perform a complicated Command
		if(!ExeComp(lineSize)) continue;
					// background command	
	 	if(!BgCmd(smash, lineSize)) continue;
					// built in commands
		ExeCmd(smash, lineSize, cmdArr);

		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdArr[0]='\0';
	}
    return 0;
}

