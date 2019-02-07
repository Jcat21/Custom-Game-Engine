#pragma once
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>


class JobManager
{
public:
    class Job
    {
    public:
        virtual void DoIt() = 0;
    };

    void Begin();
    void End();
    void AddJob(Job* pJob);
    void JobCompleted(Job* pJob);
    void WaitForJobs();
    Job* GetNextJob();

private:
    enum { NUM_WORKER = 4 };
    class Worker
    {
    public:
        void Begin(JobManager* pManager);
        void End();
        static void Loop(JobManager* pManager);

        static volatile bool s_shutDown;

    private:
        std::thread* mThread;
    };

    Worker mWorkers[NUM_WORKER];
    std::queue<JobManager::Job*> mJobs;
    std::mutex mJobMutex;
    std::atomic<int> mJobCount;
};
