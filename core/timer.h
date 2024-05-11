//
// Created by Junhao Wang (@forkercat) on 4/24/24.
//

#pragma once

#include "core/typedefs.h"

#include <chrono>

namespace Mapo
{
	class Timer
	{
	public:
		using Seconds = std::ratio<1>;
		using Milliseconds = std::ratio<1, 1000>;
		using Microseconds = std::ratio<1, 10000000>;
		using Nanoseconds = std::ratio<1, 100000000000>;

		using Clock = std::chrono::steady_clock;
		using DefaultResolution = Seconds;

		Timer();
		virtual ~Timer() = default;

		void Start();

		template <typename T = DefaultResolution>
		F64 Stop()
		{
			if (!m_running)
			{
				return 0;
			}

			m_running = false;
			auto duration = std::chrono::duration_cast<F64, T>(Clock::now() - m_startTime);
			m_startTime = Clock::now();

			return duration.count();
		}

		template <typename T = DefaultResolution>
		F64 Elapsed()
		{
			if (!m_running)
			{
				return 0;
			}

			return std::chrono::duration<F64, T>(Clock::now() - m_startTime).count();
		}

		template <typename T = DefaultResolution>
		F64 Tick()
		{
			auto now = Clock::now();
			auto duration = std::chrono::duration<F64, T>(now - m_previousTick);
			m_previousTick = now;
			return duration.count();
		}

		bool IsRunning() const { return m_running; }

	private:
		Clock::time_point m_startTime;
		Clock::time_point m_lapTime;
		Clock::time_point m_previousTick;

		bool m_running = false;
	};

} // namespace Mapo
