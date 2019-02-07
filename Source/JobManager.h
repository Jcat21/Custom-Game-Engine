#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>

class Job;
class Worker;

class JobManager
{
public:
	JobManager();
	~JobManager();

	void Begin();
	void End();
	void AddJob(Job* pJob);
	void WaitForJobs();
	Job* getNextJob();
	volatile bool mShutdownSignal;
	std::mutex mLock;
	std::atomic<int> counter;

	std::vector<Worker*> mWorkers;
	std::queue<Job*> mJobs;
};