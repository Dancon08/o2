#include "Timer.h"

namespace o2
{
	Timer::Timer()
	{
		Reset();
	}

	Timer::~Timer()
    {}
    
#ifdef WINDOWS
    void Timer::Reset()
    {
        QueryPerformanceFrequency(&mFrequency);
        QueryPerformanceCounter(&mStartTime);
        mLastElapsedTime = mStartTime.QuadPart;
    }
    
    float Timer::GetTime()
    {
        LARGE_INTEGER curTime;
        QueryPerformanceCounter(&curTime);
        
        float res = (float)((double)(curTime.QuadPart - (double)mStartTime.QuadPart)/(double)mFrequency.QuadPart);
        mLastElapsedTime = curTime.QuadPart;
        
        return res;
    }
    
    float Timer::GetDeltaTime()
    {
        LARGE_INTEGER curTime;
        QueryPerformanceCounter(&curTime);
        
        float res = (float)((double)(curTime.QuadPart - (double)mLastElapsedTime)/(double)mFrequency.QuadPart);
        mLastElapsedTime = curTime.QuadPart;
        
        return res;
    }
#endif
    
#ifdef OSX
    void Timer::Reset()
    {
    }
    
    float Timer::GetTime()
    {
        return 0;
    }
    
    float Timer::GetDeltaTime()
    {
        return 0;
    }
#endif
}
