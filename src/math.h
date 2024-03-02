#pragma once

#include <type_traits>

constexpr float PI = 3.14159265358979323846f;
constexpr float PI_2 = 1.57079632679489661923f;

template<typename T>
T divide_round_up(T a, T b)
{
	static_assert(std::is_integral_v<T>);

	return (a + b - 1) / b;
}

template<typename T>
bool is_power_of_2(T x)
{
	return !(x == 0) && !(x & (x - 1));
}
