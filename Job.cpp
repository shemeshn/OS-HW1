/*
 * Job.cpp
 *
 *  Created on: Nov 27, 2018
 *      Author: nadav
 */

#include "Job.h"

Job::Job(string name, int id) :
	name(name), pid(id), isStopped(false){
	startTime = time(NULL);
}

Job::~Job() {

}

string Job::GetName(){
	return name;
}

void Job::SetName(string newName){
	name = newName;
}

int Job::GetPid(){
	return pid;
}

double Job::GetRunningTime(){
	time_t currentTime = time(NULL);
	return (currentTime - startTime);
}

void Job::Stop(){
	isStopped = true;
}

void Job::Resume(){
	isStopped = false;
}

//**************************************************************************************
// function name: FindJob
// Description: Finds a job object with serial number num
// Parameters: job number
// Returns: if successful- a pointer to Job object, else NULL
//**************************************************************************************
Job Job::FindJob(int job_num){

}
