#pragma once

#include <type_traits>

template<typename Type = int>
Type fast_rand()
{
	static_assert(std::is_integral<Type>::value, "");

	static int seed = 'fast';

	seed = (214013 * seed + 2531011);

	return (Type)((seed >> 16) & 0x7FFF);
}

template<typename Type>
float clamp(Type min, Type value, Type max)
{
	static_assert( std::is_floating_point<Type>::value && std::is_integral<Type>::value, "");

	if (min > value)
	{
		value = min;
		return value;
	}

	if (max < value)
	{
		value = max;
		return value;
	}

	else value;
}

template<size_t MAP_RANGE, size_t FOV_RANGE>
constexpr size_t calc_static_map_index()
{
	if constexpr(MAP_RANGE % FOV_RANGE == 0)
	{
		return (MAP_RANGE / FOV_RANGE);
	}
	else
	{
		return (MAP_RANGE / FOV_RANGE) + 1;
	}
}
