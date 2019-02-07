#include "stdafx.h"
#include "Profiler.h"

/*static*/ FILE* Profiler::Timer::s_traceFile = nullptr;
/*static*/ bool Profiler::Timer::s_doComma = false;

/*static*/ Profiler *Profiler::Get()
{
	static Profiler s_theProfiler;
	return &s_theProfiler;
}

Profiler::Profiler()
{
    fopen_s(&Timer::s_traceFile, "profile.json", "wt");
    fprintf(Timer::s_traceFile, "[\n");
}

Profiler::~Profiler()
{
    fprintf(Timer::s_traceFile, "\n]\n");
    fclose(Timer::s_traceFile);

	FILE *pFile = nullptr;
	fopen_s(&pFile, "profile.txt", "wt");
	if (nullptr != pFile)
		fprintf(pFile, "name:, avg (ms), max (ms)\n");
	for (auto& iter : mTimers)
	{
		Timer *pTimer = iter.second;
		if (nullptr != pFile)
			fprintf(pFile, "%s:, %f, %f\n", pTimer->GetName().c_str(), (float)pTimer->GetAvg_ms(), (float)pTimer->GetMax_ms());
		delete pTimer;
	}
	mTimers.clear();
	fclose(pFile);
}