#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

class Profiler
{
	enum { NUM_AVERAGE = 100 };

public:
	class Timer
	{
		friend class Profiler;

	public:
		void Start()
		{
			if (false == mIsStarted)
			{
				mIsStarted = true;
				mStart = std::chrono::high_resolution_clock::now();
                uint64_t startTime = mStart.time_since_epoch().count() / 1000;
                if (s_doComma)
                {
                    fprintf(s_traceFile, ",\n");
                }
                fprintf(s_traceFile, "\t{\"name\": \"%s\", \"cat\": \"PERF\", \"ph\": \"B\", \"pid\": 1, \"tid\": 1, \"ts\": %llu}",
                    this->GetName().c_str(), startTime);
                s_doComma = true;
			}
		}

		void Stop()
		{
			if (mIsStarted)
			{
				std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
				double duration = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart).count();
				mCurrent_ms += 0.000001 * duration;
				if (mCurrent_ms > mMax_ms)
					mMax_ms = mCurrent_ms;
				mIsStarted = false;
                if (s_doComma)
                {
                    fprintf(s_traceFile, ",\n");
                }
                uint64_t endTime = end.time_since_epoch().count() / 1000;
                fprintf(s_traceFile, "\t{\"ph\": \"E\", \"pid\": 1, \"tid\": 1, \"ts\": %llu}",
                    endTime);
                s_doComma = true;
			}
		}

		void Reset()
		{
			if (mCurrent_ms > 0.0)
			{
				mTotalTimes_ms += mCurrent_ms;
				++mNumSample;
			}
			mIsStarted = false;
			mCurrent_ms = 0.0;
		}

		const std::string &GetName() const { return mName; }
		double GetTime_ms() const { return mCurrent_ms; }
		double GetMax_ms() const { return mMax_ms; }
		double GetAvg_ms() const
		{
            if (mNumSample > 0)
				return mTotalTimes_ms / mNumSample;
			return 0.0;
		}

	private:
		Timer(const std::string &name)
			: mName(name)
			, mCurrent_ms(0.0)
			, mMax_ms(0.0)
			, mIsStarted(false)
			, mTotalTimes_ms(0.0)
			, mNumSample(0)
		{
		}
		~Timer()
		{
		}

		std::string mName;
		std::chrono::high_resolution_clock::time_point mStart;
		double mCurrent_ms;
		double mMax_ms;
		bool mIsStarted;
		double mTotalTimes_ms;
		int mNumSample;

        static FILE* s_traceFile;
        static bool s_doComma;
	};

	class ScopedTimer
	{
	public:
		ScopedTimer(Timer *timer)
			: mTimer(timer)
		{
			mTimer->Start();
		}

		~ScopedTimer()
		{
			mTimer->Stop();
		}

	private:
		Timer *mTimer;
	};

	static Profiler *Get();

	Timer *GetTimer(const std::string &name)
	{
		if (mTimers.end() == mTimers.find(name))
		{
			Timer *pTimer = new Timer(name);
			mTimers[name] = pTimer;
			return pTimer;
		}
		return mTimers[name];
	}

	void ResetAll()
	{
		for (auto& iter : mTimers)
			iter.second->Reset();
	}

private:
	Profiler();
	~Profiler();
	std::unordered_map<std::string, Timer*> mTimers;
};

#define PROFILE_SCOPE(name) Profiler::ScopedTimer name_scope(Profiler::Get()->GetTimer(std::string(#name)))