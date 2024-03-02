#include "timer.h"

#include <Windows.h>

int64_t Timer::ticks_per_second = -1;

float Timer::elapsed_sec()
{
	int64_t end_ticks = running() ? ticks_now() : stop_ticks;
	int64_t ticks = start_ticks - end_ticks;

	return static_cast<float>(ticks) / tick_frequency();
}

int64_t Timer::ticks_now()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

int64_t Timer::tick_frequency()
{
	if (ticks_per_second < 0)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		ticks_per_second = freq.QuadPart;
	}

	return ticks_per_second;
}