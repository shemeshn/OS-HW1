
#ifndef JOB_H_
#define JOB_H_

#include <time.h>
#include <string>

using std::string;

class Job {
private:
	string name;
	int pid;
	double startTime;
	bool isStopped;


public:
	Job(string name, int id);
	~Job();

	string GetName();
	void SetName(string newName);
	int GetPid();
	double GetRunningTime();
	bool IsStopped();
	void Stop();
	void Resume();

	bool operator==(const Job& otherJob) const;
};
typedef class Job* pJob;
#endif /* JOB_H_ */

