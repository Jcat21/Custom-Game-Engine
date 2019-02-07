#pragma once
#include <thread>

class JobManager;

static JobManager* mManager;
class Worker
{
public:
	Worker(JobManager* pManager);
	~Worker();

	void Begin();
	void End();
	static void Loop();
private:
	std::thread* mThread;
};