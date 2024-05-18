//
// Created by Junhao Wang (@forkercat) on 4/20/24.
//

#pragma once

#include "core/core.h"

namespace Mapo
{
	class Camera
	{
	public:
		virtual ~Camera() = default;

		Camera()
		{
		}

		Camera(const Matrix4& projection)
			: m_projection(projection)
		{
		}

		const Matrix4& GetProjectionMatrix() const { return m_projection; }
		Matrix4&	   GetProjectionMatrix() { return m_projection; }

	protected:
		Matrix4 m_projection{ 1.0f };
	};

} // namespace Mapo
