#pragma once

#include <cstdlib>

#define UNUSED(x) ((void)x)

#ifdef _DEBUG
	#define ASSERT(x) \
		do { if (!(x)) { __debugbreak(); } } while(0)
#else
	#define ASSERT(x)
#endif

template<typename T, size_t N>
constexpr size_t array_size(const T(&)[N])
{
	return N;
}
