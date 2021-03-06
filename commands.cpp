//		commands.c
//********************************************
#include "commands.h"
#include "signals.h"

using namespace std;

char prevWD[MAX_LINE_SIZE];

//*****************************************************************************
//*************** Smash functions
//*****************************************************************************

//**************************************************************************************
// function name: UpdateJobsList
// Description: when a child process dies, removes it from job list.
// Parameters: Smash struct
// Returns: None
//**************************************************************************************
void UpdateJobsList(Smash& smash){

	int status = 0;
	int deadChildPid = waitpid(-1, &status, WNOHANG);

	while(deadChildPid > 0){	// for all child processes that finished running
		// Find job in jobs_list
		Job deadChildJob("", 0);
		for(list<Job>::iterator it=smash.job_list.begin(); it!=smash.job_list.end(); ++it){
			if(it->GetPid() == deadChildPid){
				deadChildJob = (*it);
			}
		}

		if(deadChildJob.IsStopped()){	// In case we caught a suspended process
			deadChildPid = waitpid(-1, &status, WNOHANG);
			continue;
		} else {	// Finally, caught a terminated process
			smash.job_list.remove(deadChildJob);
		}
		deadChildPid = waitpid(-1, &status, WNOHANG);
	}
}

//**************************************************************************************
// function name: InsertJob
// Description: creates a new job object and inserts it to job list.
// Parameters: Smash struct, job's name, job's pid
// Returns: None
//**************************************************************************************
void InsertJob(Smash& smash, string name, int pid){
	Job newJob(name, pid);
	smash.job_list.push_back(newJob);
	return;
}

//**************************************************************************************
// function name: PrintJobs
// Description: prints a list of processes that run on bg.
// Parameters: Smash struct
// Returns: None
//**************************************************************************************

void PrintJobs(Smash& smash){

	int counter = 1;
    for (list<Job>::iterator it = smash.job_list.begin(); it != smash.job_list.end() ; ++it) {
        cout << "[" << counter << "] " << it->GetName() << " ";
        cout << it->GetPid() << " " << it->GetRunningTime() << " secs ";
        if((*it).IsStopped()){
        	cout << "(Stopped)";
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

//**************************************************************************************
// function name: BringToFg
// Description: beings a bg process to fg
// Parameters: Smash struct, index of job in job list
// Returns: None
//**************************************************************************************
void BringToFg(Smash& smash, int jobListIndex){
	jobListIndex--;		// Counting from 0, not 1
	if(jobListIndex < 0 || jobListIndex >= smash.job_list.size()){
		perror("smash error: >Invalid job index");
		return;
	}

	Job jobToFg("", 0);
	// Find relevant Job in list
	for(list<Job>::iterator it = smash.job_list.begin(); it != smash.job_list.end(); ++it){
		if(0 == jobListIndex){		// Found it!
            //sending SIGCONT if necessary
            if(it->IsStopped()){
                if (signal_handler(SIGCONT, it->GetPid(), *it)==FAILURE)
                    perror("smash error: >signal failed");
            }
			jobToFg = (*it);
			break;
		}
		jobListIndex--;
	}

	int status = 0;

	//setting signal handlers to child process
	signal(SIGTSTP, ctrlZ_handler);
	signal(SIGINT, ctrlC_handler);

	cout << jobToFg.GetName() << endl;

	smash.fg_job_pid = jobToFg.GetPid();
	smash.job_list.remove(jobToFg);
	if(-1 == waitpid(jobToFg.GetPid(), &status, WUNTRACED)){
		perror("smash error: >waitpid failed");
	}
	return;
}

//**************************************************************************************
// function name: KillAndQuit
// Description: kills all child processes before killing the smash process and quits
// Parameters: none
// Returns: none
//**************************************************************************************
static void KillAndQuit(Smash& smash){
	int index = 1;
	int status;

	for(list<Job>::iterator it = smash.job_list.begin(); it != smash.job_list.end(); ++it){

		bool terminated = false;
		cout << "[" << index << "]" << it->GetName() << " - Sending SIGTERM... ";
		time_t startTime = time(NULL);

		do{
			kill(it->GetPid(), SIGTERM);
			pid_t result = waitpid(it->GetPid(), &status, WNOHANG);		// check child status
			if(result != 0 && result != -1){	// if child terminated
				cout << "Done." << endl;
				terminated = true;
			}
		} while(!terminated && time(NULL) - startTime < 5);

		if(!terminated){		// process resisted SIGTERM, sending SIGKILL
			cout << "(5 sec passed) Sending SIGKILL... ";
			kill(it->GetPid(), SIGKILL);
			cout << "Done." << endl;
		}
		index++;
	}

	// Clean up
	smash.job_list.clear();
	smash.cmd_list.clear();
	exit(1);
}

//**************************************************************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(Smash& smash, char* lineSize, char* cmdString)
{
    using namespace std;
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE]; // Used in pwd command
	const char* delimiters = " \t\n";
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
    	Job job("",0);
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
            //checking if signum and job_num are integers or 0
            if((signum==0)||(job_num==0)) {
                illegal_cmd = true;
                arg_err = true;
            }
            //checking if a job with serial number job_num was found
			int counter = 1;
            bool found = false;
			for(list<Job>::iterator it=smash.job_list.begin(); it!=smash.job_list.end(); ++it){
				if(counter==job_num){
					if (signal_handler(signum, it->GetPid(), *it)==FAILURE){
						cout<<"smash error:> kill "<<job_num<<"- cannot send signal"<<endl;
					}
					found = true;
					break;
				}
				counter++;
			}
            if( found == false){
                cout<<"smash error:> kill "<<job_num<<"- job does not exist"<<endl;
            }
        }
    }
    /*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
		switch(num_arg){
			case 0:
				BringToFg(smash, smash.job_list.size());
				break;

			case 1:
				if(0 == atoi(args[1])){		// atoi failed, not a number
					illegal_cmd = true;
					break;
				}
				BringToFg(smash, atoi(args[1]));
				break;

			default:
				illegal_cmd = true;
		}
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
                for(list<Job>::iterator it=smash.job_list.begin(); it!=smash.job_list.end(); ++it){
                    if((*it).GetPid()){
                        if(signal_handler(SIGCONT, job_pid, (*it))==FAILURE)
                            perror("smash error: >");
                    }
                    cout<<(*it).GetName()<<endl;
                }
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
            int counter = 1;
            job_pid = -1;
            //finding job with serial number job_num and resumes it
            if(job_num>0){
                int counter = 1;
                for(list<Job>::iterator it=smash.job_list.begin(); it!=smash.job_list.end(); ++it){
                    if(counter==job_num){
                        if(it->IsStopped()){
                            if(signal_handler(SIGCONT, it->GetPid(), *it)==SUCCESS)
								cout<<(*it).GetName()<<endl;
                            else
								perror("smash error: >");
                        }
                    }
                    counter++;
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
   					KillAndQuit(smash);
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
 		ExeExternal(args, cmdString, smash);
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
void ExeExternal(char *args[MAX_ARG], char* cmdString, Smash& smash)
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
            smash.fg_job_pid = pID;
            smash.fg_job_name = args[0];
            if(-1 == waitpid(pID, &status, WUNTRACED)){
            	perror("smash error: > waitpid() failed");
            }
            smash.fg_job_pid = 0;
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
		return 0;
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
int BgCmd(Smash& smash, char* lineSize)
{
	char* Command;
	const char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		// Add your code here (execute a in the background)

		// Parse command
		Command = strtok(lineSize, delimiters);
		if (Command == NULL)
			return 0;
	   	args[0] = Command;
		for (int i=1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
		}

		// Creating process to run command
		int pid = 0;
		switch(pid = fork()){
			case -1:	// fork failed
				perror("smash error: > Could not create a new process");

			case 0 :	// Child Process runs the command
				setpgrp();
				execvp(args[0], args);
				perror("child smash error: > External command execution failed");
                printf("smash > ");// prints smash before exit, otherwise prints an empty line
				exit(1);
				break;

			default:	// Father process inserts job to smash and continues as usual
				InsertJob(smash, lineSize, pid);
				return 0;
		}

	}
	return -1;
}
