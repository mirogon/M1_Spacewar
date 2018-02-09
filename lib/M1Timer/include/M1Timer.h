#pragma once
#include <stdint.h>
#include <chrono>

namespace m1 {

	class C_Timer
	{
	public:

		C_Timer();
		~C_Timer() = default;

		void RestartTimer();

		//Return the delta time in milliseconds
		uint32_t GetTimeSinceStart_milliseconds();
		uint64_t GetTimeSinceStart_microseconds();


	private:

		std::chrono::high_resolution_clock::time_point startTime;
		std::chrono::high_resolution_clock::duration timeSinceStart;

	};

	inline uint32_t C_Timer::GetTimeSinceStart_milliseconds()
	{
		timeSinceStart = std::chrono::high_resolution_clock::now() - startTime;
		return std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStart).count();
	}

	inline uint64_t C_Timer::GetTimeSinceStart_microseconds()
	{
		timeSinceStart = std::chrono::high_resolution_clock::now() - startTime;
		return std::chrono::duration_cast<std::chrono::microseconds>(timeSinceStart).count();
	}

}

