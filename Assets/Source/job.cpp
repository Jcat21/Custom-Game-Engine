#include "stdafx.h"
#include "job.h"


/*static*/ volatile bool JobManager::Worker::s_shutDown = false;


void JobManager::Begin()
{
    mJobCount = 0;
    for (int i = 0; i < NUM_WORKER; ++i)
        mWorkers[i].Begin(this);
}

void JobManager::End()
{
    Worker::s_shutDown = true;
    for (int i = 0; i < NUM_WORKER; ++i)
        mWorkers[i].End();
}

void JobManager::AddJob(Job* pJob)
{
    mJobMutex.lock();
    mJobs.push(pJob);
    ++mJobCount;
    mJobMutex.unlock();
}

void JobManager::JobCompleted(JobManager::Job* pJob)
{
    --mJobCount;
}

JobManager::Job* JobManager::GetNextJob()
{
    Job* pJob = nullptr;
    mJobMutex.lock();
    if (false == mJobs.empty())
    {
        pJob = mJobs.front();
        mJobs.pop();
    }
    mJobMutex.unlock();
    return pJob;
}

void JobManager::WaitForJobs()
{
    while (mJobCount > 0)
        Sleep(1);
}

void JobManager::Worker::Begin(JobManager* pManager)
{
    mThread = new std::thread(Loop, pManager);
}

void JobManager::Worker::End()
{
    mThread->join();
    delete mThread;
}

/*static*/ void JobManager::Worker::Loop(JobManager* pManager)
{
    while (false == s_shutDown)
    {
        Job* pJob = pManager->GetNextJob();
        if (pJob)
        {
            pJob->DoIt();
            pManager->JobCompleted(pJob);
        }
        else
        {
            Sleep(1);
        }
    }
}


