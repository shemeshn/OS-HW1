/*
 * Job.cpp
 *
 *  Created on: Nov 27, 2018
 *      Author: nadav
 */

#include "Job.h"

using namespace std;
//**************************************************************************************
// function name: Job
// Description: Job class constructor
//**************************************************************************************
Job::Job(string name, int id) :
	name(name), pid(id), isStopped(false){
	startTime = time(NULL);
}

//**************************************************************************************
// function name: ~Job
// Description: Job class destructor
//**************************************************************************************
Job::~Job() {

}

//**************************************************************************************
// function name: GetName
// Description: gets Job's name
//**************************************************************************************
string Job::GetName(){
	return name;
}

//**************************************************************************************
// function name: SetName
// Description: sets Job's name to newName
//**************************************************************************************
void Job::SetName(string newName){
	name = newName;
}

//**************************************************************************************
// function name: GetPid
// Description: gets Job's PID
//**************************************************************************************
int Job::GetPid(){
	return pid;
}

//**************************************************************************************
// function name: GetRunningTime
// Description: gets Job's running time
//**************************************************************************************
double Job::GetRunningTime(){
	time_t currentTime = time(NULL);
	return (currentTime - startTime);
}

//**************************************************************************************
// function name: IsJobStopped
// Description: returns true if job is stopped, false otherwise
//**************************************************************************************
bool Job::IsStopped(){
	return isStopped;
}

//**************************************************************************************
// function name: Stop
// Description: stops a running job
//**************************************************************************************
void Job::Stop(){
	isStopped = true;
}

//**************************************************************************************
// function name: Resume
// Description: resumes a stopped job
//**************************************************************************************
void Job::Resume(){
	isStopped = false;
}

//**************************************************************************************
// function name: operator==
// Description: overloading of operator == for job class
//**************************************************************************************
bool Job::operator==(const Job& otherJob) const {
	return (pid == otherJob.pid);
}
