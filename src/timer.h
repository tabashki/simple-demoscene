#pragma once

#include "math.h"

#include <cstdint>

struct Timer
{
	int64_t start_ticks;
	int64_t stop_ticks;

public:
	void start()
	{
		start_ticks = ticks_now();
		stop_ticks = -1;
	}

	void stop()
	{
		stop_ticks = ticks_now();
	}

	bool running() { return (stop_ticks < 0); }

	float elapsed_sec();

private:
	static int64_t ticks_now();
	static int64_t tick_frequency();

	static int64_t ticks_per_second;
};