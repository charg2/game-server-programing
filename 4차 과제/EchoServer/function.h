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


