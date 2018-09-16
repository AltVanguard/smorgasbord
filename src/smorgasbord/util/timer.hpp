#ifndef SMORGASBORD_TIMER_HPP
#define SMORGASBORD_TIMER_HPP

#ifdef WIN32 // Windows
	#include <windows.h>
#else // Unix
	#include <sys/time.h>
#endif

/*

class Timer
-----------

Reset() resets timer so GetSeconds() returns the elapsed time since its
preceeding Start() call, ignoring pauses.
To restart the timer, call Reset(), then Start(). Alternatively, you can
restart the timer in a single call by calling SetSeconds(0).

*/

namespace Smorgasbord {

#ifdef WIN32 // Windows ////////////////////////////////////////////////

class Timer
{
private:

	LARGE_INTEGER start, end, accumulated; // time markers
	double tickPeriod;
	bool isRunning;
	
public: // constructor

	Timer()
	{
		start.QuadPart = 0;
		end.QuadPart = 0;
		accumulated.QuadPart = 0;
		
		LARGE_INTEGER tickFrequency;
		QueryPerformanceFrequency(&tickFrequency);
		tickPeriod = 1.0 / tickFrequency.QuadPart;
		
		isRunning = false;
	}
	
	~Timer() {}
	
public:

	inline bool IsRunning()
	{
		return isRunning;
	}
	
	// Starts/continues timer
	inline void Start()
	{
		isRunning = true;
		QueryPerformanceCounter(&start);
	}
	
	// Stops timer
	inline void Stop()
	{
		QueryPerformanceCounter(&end);
		isRunning = false;
	}
	
	// Pauses timer
	inline void Pause()
	{
		if (isRunning)
		{
			QueryPerformanceCounter(&end);
		}
		
		accumulated.QuadPart += end.QuadPart - start.QuadPart;
		end.QuadPart = start.QuadPart;
        isRunning = false;
	}
	
	// Resets timer so GetSeconds() returns the elapsed time since
	// its preceeding Start() call, ignoring pauses.
	inline void Reset()
	{
		accumulated.QuadPart = 0;
	}
	
	// Sets elapsed time in seconds
	inline void SetSeconds(double seconds)
	{
		accumulated.QuadPart = (LONGLONG)(seconds / tickPeriod);
		
		if (isRunning)
		{
			QueryPerformanceCounter(&start);
		}
	}
	
	// Gets elapsed time in seconds
	inline double GetSeconds()
	{
		if (isRunning)
		{
			QueryPerformanceCounter(&end);
		}
		
		return (end.QuadPart - start.QuadPart) * tickPeriod;
	}
};

#else // Unix //////////////////////////////////////////////////////////

class Timer
{
private:
	timeval start, end, accumulated; // time markers
	bool isRunning;

public: // constructor
	Timer()
	{
		start.tv_sec = 0;
		start.tv_usec = 0;
		accumulated = end = start;

		isRunning = false;
	}

	~Timer() {}

public:
	inline bool IsRunning()
	{
		return isRunning;
	}

	// Starts/continues timer
	inline void Start()
	{
		isRunning = true;
		gettimeofday(&start, nullptr);
	}

	// Stops timer
	inline void Stop()
	{
		gettimeofday(&end, nullptr);
		isRunning = false;
	}

	// Pauses timer
	inline void Pause()
	{
		if (isRunning)
		{
			gettimeofday(&end, nullptr);
		}

		accumulated.tv_sec += end.tv_sec - start.tv_sec;
		accumulated.tv_usec += end.tv_usec - start.tv_usec;
		end = start;
		isRunning = false;
	}

	// Resets timer so GetSeconds() returns the elapsed time since
	// its preceeding Start() call, ignoring pauses.
	inline void Reset()
	{
		accumulated.tv_sec = accumulated.tv_usec = 0;
	}

	// Sets elapsed time in seconds
	inline void SetSeconds(double seconds)
	{
		accumulated.tv_sec = (int)seconds;
		accumulated.tv_usec =
			(int)(1000000.0 * (seconds - (double)accumulated.tv_sec));

		if (isRunning)
		{
			gettimeofday(&start, nullptr);
		}
	}

	// Gets elapsed time in seconds
	inline double GetSeconds()
	{
		if (isRunning)
		{
			gettimeofday(&end, nullptr);
		}

		return (end.tv_sec - start.tv_sec + accumulated.tv_sec)
			+ (end.tv_usec - start.tv_usec + accumulated.tv_usec) * 0.000001;
	}
};

#endif

}

#endif // SMORGASBORD_TIMER_HPP
