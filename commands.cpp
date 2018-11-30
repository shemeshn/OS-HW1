//		commands.c
//********************************************
#include "commands.h"
#include "signals.h"

using namespace std;

char prevWD[MAX_LINE_SIZE];

//*****************************************************************************
//*************** Smash functions
//*****************************************************************************

void InsertJob(Smash smash, string name, int pid){
	Job newJob(name, pid);
	smash.job_list.push_back(newJob);
	return;
}

void PrintJobs(Smash smash){

	int counter = 1;
    for (list<Job>::iterator it = smash.job_list.begin(); it != smash.job_list.end() ; ++it) {
        cout << "[" << counter << "] " << it->GetName() << " ";
        cout << it->GetPid() << " " << it->GetRunningTime() << " secs";
        if(it->IsJobStopped()){
        	cout << " (Stopped)";
        }
        cout << endl;
        counter++;
    }
}

//**************************************************************************************
// function name: ChangeDirectory
// Description: changes directory.
// Parameters: path to new directory
// Returns: if successful- SUCCESS, else FAILURE
//**************************************************************************************
Result ChangeDirectory(char* path){
	char pwd[MAX_LINE_SIZE], auxWD[MAX_LINE_SIZE];
	getcwd(pwd, MAX_LINE_SIZE);
	strcpy(auxWD, prevWD);

	if(!strcmp(path, "-")){
		strcpy(pwd, auxWD);
		getcwd(prevWD, MAX_LINE_SIZE);
		chdir(pwd);
		return SUCCESS;
	}

	int cdResult = chdir(path);

	if(0 != cdResult){  // chdir command failed
		memset(prevWD, 0x0, MAX_LINE_SIZE);
		strcpy(prevWD, auxWD);
		return FAILURE;
	}
	else {	// chdir command succeeded
		strcpy(prevWD, pwd);
		getcwd(pwd, MAX_LINE_SIZE);
		return SUCCESS;
	}
}

static void KillAndQuit(){
	// TODO: complete this function
	exit(1);
}
//**************************************************************************************
// function name: FindJobPID
// Description: Finds a job object with serial number job_num
// Parameters: job number, smash struct
// Returns: if successful- a pointer to Job object, else NULL
//**************************************************************************************

int FindJobPID(int job_num, Smash smash){
	int counter = 1;
    for(list<Job>::iterator it=smash.job_list.begin(); it!=smash.job_list.end(); ++it){
        if(counter==job_num){
            return (*it).GetPid();
        }
    }
    return -1;
}

//**************************************************************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(Smash smash, char* lineSize, char* cmdString)
{
    using namespace std;
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE]; // Used in pwd command
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
	bool arg_err = false; // argument error
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
		switch (num_arg) {
			case 1:
				if (SUCCESS != ChangeDirectory(args[1])){
					printf("smash error: > \"%s\" - path not found\n", args[1]);
				}
				if(!strcmp(args[1], "-")){
					char* cwd = getcwd(pwd, MAX_LINE_SIZE);
					printf("%s\n", cwd);
				}
				break;

			default:
				illegal_cmd = true;
		}
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
            arg_err = true;
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
            arg_err = true;
        }
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
		switch(num_arg){
			case 0:
				PrintJobs(smash);
				break;

			default:
				illegal_cmd = true;
		}
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
    else if (!strcmp(cmd, "kill"))
    {
        int signum;
        int pid;
        int job_num;
		string sig_name = args[1];

        //checking if argument number is valid and signum starts with '-'
        if((num_arg!=2)||(sig_name.compare(0,1,"-")!=0))
        {
            illegal_cmd = true;
            arg_err = true;
        }
        else
        {
            //converting job_num to integer
			job_num = atoi(args[2]);
            //removing the '-'
            sig_name.erase(sig_name.begin());
            //converting string to integer
            signum = atoi(sig_name.c_str());
            //conversion failed
            pid = FindJobPID(job_num, smash);
            //checking if signum and job_num are integers or 0
            if((signum==0)||(job_num==0)) {
                illegal_cmd = true;
                arg_err = true;
            }
            //checking if a job with serial number job_num was found
            else if(pid==-1){
                cout<<"smash error:> kill "<<job_num<<"- job does not exist"<<endl;
            }
            else if(signal_handler(signum, pid)==FAILURE)
                cout<<"smash error:> kill "<<job_num<<"- cannot send signal"<<endl;
        }
    }
    /*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		int job_pid;
		//resuming last stopped job in background
		if(num_arg==0)
		{
			if(smash.stopped_jobs.size()>0){
				//printing the name of the last job that was stopped
				cout<<smash.stopped_jobs.back().GetName()<<endl;
				job_pid = smash.stopped_jobs.back().GetPid();
				//finding job in job list and changing it's status
				for (list<Job>::iterator it = smash.job_list.begin(); it != smash.job_list.end() ; ++it){
					if((*it).GetPid()==job_pid){
                        (*it).Resume();
					}
				}
                if(signal_handler(SIGCONT, job_pid)==FAILURE)
                    perror("smash error: >");
			}
			else{
				//printing most recent job's name
				cout<<smash.job_list.back().GetName()<<endl;
			}
		}
		//resuming a job with a specific serial number in bg
		else if(num_arg == 1)
		{
            int job_num = atoi(args[1]);
            int job_pid = -1;
            int counter = 1;
            //finding job with serial number job_num and resumes it
            if(job_num>0){
                for (list<Job>::iterator it = smash.job_list.begin(); it != smash.job_list.end() ; ++it){
                    if(counter == job_num) {
                        if ((*it).IsJobStopped()) {
                            (*it).Resume();
                            job_pid = (*it).GetPid();
                        }
                        break;
                    }
                }
                //if a stopped job was found, sending SIGCONT
                if(job_pid!=-1){
                    if(signal_handler(SIGCONT, job_pid)==FAILURE)
                        perror("smash error: >");
                }
            }
            else
            {
                illegal_cmd = true;
                arg_err = true;
            }
		}
		//wrong number of arguments
		else
		{
            illegal_cmd = true;
            arg_err = true;
		}
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
            arg_err = true;
        }
        //renaming the file
        else {
            //checking if renaming was successful
            if(rename(args[1], args[2])==-1) {
                illegal_cmd = true;
                sys_err = true;
            }
            cout<<args[1]<<" has been renamed to "<<args[2]<<endl;
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
        if (arg_err)
            printf("smash error: > \"%s\"\n", ARG_ERR);
        //In case of system error
        else if (sys_err)
            perror("smash error: >");
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
	switch(pID = fork())
	{
	    case -1:
			// Add your code here (error)

			perror("smash error: > Could not create a new process");
		case 0 :	// Child Process
            setpgrp();
            execvp(args[0], args);
			perror("smash error: > External command execution failed");
			exit(1);
			break;

		default:	// Father process: wait for son process to die
            int status;
            if(-1 == waitpid(pID, &status, WUNTRACED)){
            	perror("smash error: > waitpid() failed");
            }
            break;
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
int BgCmd(Smash smash, char* lineSize)
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
