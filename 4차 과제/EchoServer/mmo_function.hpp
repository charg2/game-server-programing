#pragma once
template<size_t MAP_RANGE, size_t FOV_RANGE>
constexpr size_t calc_static_map_index()
{
	if (MAP_RANGE % FOV_RANGE == 0)
	{
		return (MAP_RANGE / FOV_RANGE);
	}
	else
	{
		return (MAP_RANGE / FOV_RANGE) + 1;
	}
}
