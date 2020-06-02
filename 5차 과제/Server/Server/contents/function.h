#pragma once


float clamp(float min, float value, float max)
{
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


uint32_t fast_rand()
{
	static int seed = 'fast';

	seed = (214013 * seed + 2531011);

	return ((seed >> 16) & 0x7FFF);
}

