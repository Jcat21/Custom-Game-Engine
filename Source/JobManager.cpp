#include "stdafx.h"
#include "JobManager.h"
#include "Worker.h"
#include "Job.h"

JobManager::JobManager()
{
	mShutdownSignal = false;
	for (int i = 0; i < 4; ++i)
	{
		Worker* w = new Worker(this);
		mWorkers.push_back(w);
	}
	counter = 0;
}

JobManager::~JobManager()
{
}

void JobManager::Begin()
{
	for (unsigned int i = 0; i < mWorkers.size(); ++i)
	{
		mWorkers[i]->Begin();
	}
}

void JobManager::End()
{
	mShutdownSignal = true;
	for (unsigned int i = 0; i < mWorkers.size(); ++i)
	{
		mWorkers[i]->End();
	}
}

void JobManager::AddJob(Job * pJob)
{
	mLock.lock();
	mJobs.push(pJob);
	counter++;
	mLock.unlock();
}

void JobManager::WaitForJobs()
{
	while (counter > 0)
	{
		Sleep(1);
	}
}

Job* JobManager::getNextJob() {
	if (!mJobs.empty()) {
		Job* job = mJobs.front();
		mJobs.pop();
		return job;
	}

	return nullptr;
}
