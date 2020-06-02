#pragma once

#include <Windows.h>
#include <cstdint>

namespace c2::util
{
	class LowPresicionTimer
	{
	public:
		LowPresicionTimer() : delta_tick{}
		{
			prev_tick = current_tick = GetTickCount64();
		}

		void update()
		{
			prev_tick = current_tick;
			current_tick = GetTickCount64();
			delta_tick = current_tick - prev_tick;
		}

		constexpr uint64_t get_delta_tick() const noexcept
		{
			return current_tick;
		}

		constexpr float get_delta_time_signle() const
		{
			return delta_tick / 1000.f;
		}

		constexpr double get_delta_time_double() const
		{
			return delta_tick / 1000.;
		}

	public:
		uint64_t current_tick;
		uint64_t prev_tick;
		uint64_t delta_tick;
	};

}