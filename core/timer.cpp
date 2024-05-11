//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#include "timer.h"

namespace Mapo
{
	Timer::Timer()
		: m_startTime{ Clock::now() },
		  m_previousTick{ Clock::now() }
	{
	}

	void Timer::Start()
	{
		if (!m_running)
		{
			m_running = true;
			m_startTime = Clock::now();
		}
	}

} // namespace Mapo
