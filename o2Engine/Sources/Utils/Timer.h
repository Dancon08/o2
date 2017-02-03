#pragma once

#ifdef WINDOWS
#include <Windows.h>
#endif

#ifdef OSX
#include <sys/time.h>
#endif

namespace o2
{
	// ------------------------------------
	// Timer class, using for counting time
	// ------------------------------------
	class Timer
	{
	public:
		// Default constructor
		Timer();

		// Destructor
		~Timer();

		// Resets time
		void Reset();

		// Returns time in seconds from last Reset() call
		float GetTime();

		// Returns time in seconds from last Reset() or GetElapsedTime() call
		float GetDeltaTime();

	protected:
        
#ifdef WINDOWS
		LONGLONG      mLastElapsedTime;
		LARGE_INTEGER mFrequency;
		LARGE_INTEGER mStartTime;
#endif
        
#ifdef OSX
        struct timeval mLastElapsedTime;
        struct timeval mStartTime;
#endif
	};
}
