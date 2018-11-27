//		commands.c
//********************************************
#include "commands.h"
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
using namespace std;

static void KillAndQuit(){
	// TODO: complete this function
	exit(1);
}

int ExeCmd(void* jobs, char* lineSize, char* cmdString, Smash smash)
{
    using namespace std;
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE]; // Used in pwd command
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
	bool arg_num_err = false; // argument error
	bool sys_err = false; //system error
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++;
	}



/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
	    if (num_arg == 0)
	    {
            getcwd(pwd, sizeof(pwd));
            printf("%s\n", pwd);
        }
        else //in case of wrong number of arguments
        {
            illegal_cmd = true;
            arg_num_err = true;
        }
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
        if (num_arg == 0)
        {
            //printing history list
            for (list<string>::iterator it = smash.cmd_list.begin(); it != smash.cmd_list.end() ; ++it) {
                cout<<*it<<endl;
            }
        }
        else //in case of wrong number of arguments
        {
            illegal_cmd = true;
            arg_num_err = true;
        }
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
 		
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
 		switch (num_arg){
			case 0:
				printf("smash pid is %d\n", getpid());
				break;

			default:
				illegal_cmd = true;
 		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		switch(num_arg){
   			case 0:
   				exit(1);
   				break;

   			case 1:
   				if(!strcmp(args[1], "kill")){
   					KillAndQuit();
   				} else {
   					illegal_cmd = true;
   				}
   				break;

   			default:
   				illegal_cmd = true;
   		}
	} 

	/*************************************************/
    else if (!strcmp(cmd, "mv"))
    {
        struct stat buf;
        //checking for argument error
        if (num_arg!=2) {
            illegal_cmd = true;
            arg_num_err = true;
        }
        //renaming the file
        else {
            //checking if renaming was successful
            if(rename(args[1], args[2])==-1) {
                illegal_cmd = true;
                sys_err = true;
            }
        }
    }
        /*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd)
	{
		//In case of wrong number of arguments
        if (arg_num_err)
            printf("smash error: > \"%s\"\n", ARG_ERR);
        //In case of system error
        else if (sys_err)
            perror("An error has occurred");
        else
            printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID =0;
	int tmp;
	switch(pID = fork())
	{
	    case -1:
			// Add your code here (error)
			perror("failure");
		case 0 :
            // Child Process
            setpgrp();

			// Add your code here (execute an external command)
			// execv(cmdString);
			perror("External command execution failed");

		default:
            // Add your code here
			wait(&tmp);
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs)
{

	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		// Add your code here (execute a in the background)
					
		/* 
		your code
		*/
		
	}
	return -1;
}

//test
