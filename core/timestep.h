//
// Created by Junhao Wang (@forkercat) on 5/6/24.
//

#pragma once

#include "core/typedefs.h"

namespace Mapo
{
	class Timestep final
	{
	public:
		Timestep(F32 time = 0.0f)
			: m_time(time)
		{
		}

		MP_FORCE_INLINE operator float() const
		{
			return m_time;
		}

		MP_FORCE_INLINE F32 GetSeconds() const
		{
			return m_time;
		}

		MP_FORCE_INLINE F32 GetMilliseconds() const
		{
			return m_time * 1000.0f;
		}

	private:
		F32 m_time = 0.0f;
	};

} // namespace Mapo
