#include "stdafx.h"
#include "Worker.h"
#include "JobManager.h"
#include "Job.h"
#include <chrono>
#include <mutex>

Worker::Worker(JobManager* pManager)
{
	mManager = pManager;
}

Worker::~Worker()
{
	delete mThread;
}

void Worker::Begin()
{
	mThread = new std::thread(Worker::Loop);
}

void Worker::End()
{
	mThread->join();
}

void Worker::Loop()
{
	Job* job;
	while (!(mManager->mShutdownSignal))
	{
		mManager->mLock.lock();
		job = mManager->getNextJob();
		mManager->mLock.unlock();
		if (job != nullptr)
		{
			job->DoIt();
			mManager->counter--;
		}

	}
	return;
}
