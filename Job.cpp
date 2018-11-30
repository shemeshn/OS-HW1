/*
 * Job.cpp
 *
 *  Created on: Nov 27, 2018
 *      Author: nadav
 */

#include "Job.h"

using namespace std;
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

bool Job::IsJobStopped(){
	return isStopped;
}

void Job::Stop(){
	isStopped = true;
}

void Job::Resume(){
	isStopped = false;
}
