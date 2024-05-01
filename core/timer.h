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
		Timer() = default;

		void Reset()
		{
			m_start = std::chrono::high_resolution_clock::now();
		}

		F32 GetSeconds() const
		{
			using namespace std::chrono;
			return duration_cast<nanoseconds>(high_resolution_clock::now() - m_start).count() * 0.001f * 0.001f * 0.001f;
		}

		F32 GetMilliseconds() const
		{
			return GetSeconds() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	};
	
} // namespace Mapo
