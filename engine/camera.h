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
		void SetOrthographicProjection(F32 left, F32 right, F32 top, F32 bottom, F32 near, F32 far);
		void SetPerspectiveProjection(F32 fovy, F32 aspect, F32 near, F32 far);

		void SetViewDirection(Vector3 position, Vector3 direction, Vector3 up = { 0.0f, -1.0f, 0.0f });
		void SetViewTarget(Vector3 position, Vector3 target, Vector3 up = { 0.0f, -1.0f, 0.0f });
		void SetViewYXZ(Vector3 position, Vector3 rotation);

		const Matrix4& GetProjection() const { return m_projectionMatrix; }
		const Matrix4& GetView() const { return m_viewMatrix; }

	private:
		Matrix4 m_projectionMatrix{ 1.0f };
		Matrix4 m_viewMatrix{ 1.0f };
	};

} // namespace Mapo
