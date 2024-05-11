//
// Created by Junhao Wang (@forkercat) on 4/20/24.
//

#include "camera.h"

namespace Mapo
{
	void Camera::SetOrthographicProjection(F32 left, F32 right, F32 top, F32 bottom, F32 near, F32 far)
	{
		m_projectionMatrix = Matrix4{ 1.0f };
		m_projectionMatrix[0][0] = 2.0f / (right - left);
		m_projectionMatrix[1][1] = 2.0f / (bottom - top);
		m_projectionMatrix[2][2] = 1.0f / (far - near);

		m_projectionMatrix[3][0] = -(right + left) / (right - left);
		m_projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		m_projectionMatrix[3][2] = -near / (far - near);
	}

	void Camera::SetPerspectiveProjection(F32 fovy, F32 aspect, F32 near, F32 far)
	{
		MP_ASSERT(MathOp::Abs(aspect - std::numeric_limits<F32>::epsilon()) > 0.0f, "");

		const F32 tanHalfFovy = tan(fovy / 2.0f);
		m_projectionMatrix = Matrix4{ 0.0f };
		m_projectionMatrix[0][0] = 1.0f / (aspect * tanHalfFovy);
		m_projectionMatrix[1][1] = 1.0f / (tanHalfFovy);
		m_projectionMatrix[2][2] = far / (far - near);
		m_projectionMatrix[2][3] = 1.0f;
		m_projectionMatrix[3][2] = -(far * near) / (far - near);
	}

	void Camera::SetViewDirection(Vector3 position, Vector3 direction, Vector3 up)
	{
		// Orthonormal basis
		const Vector3 w{ MathOp::Normalize(direction) };
		const Vector3 u{ MathOp::Normalize(MathOp::Cross(w, up)) };
		const Vector3 v{ MathOp::Cross(w, u) };

		m_viewMatrix = Matrix4{ 1.f };
		m_viewMatrix[0][0] = u.x;
		m_viewMatrix[1][0] = u.y;
		m_viewMatrix[2][0] = u.z;
		m_viewMatrix[0][1] = v.x;
		m_viewMatrix[1][1] = v.y;
		m_viewMatrix[2][1] = v.z;
		m_viewMatrix[0][2] = w.x;
		m_viewMatrix[1][2] = w.y;
		m_viewMatrix[2][2] = w.z;
		m_viewMatrix[3][0] = -MathOp::Dot(u, position);
		m_viewMatrix[3][1] = -MathOp::Dot(v, position);
		m_viewMatrix[3][2] = -MathOp::Dot(w, position);
	}

	void Camera::SetViewTarget(Vector3 position, Vector3 target, Vector3 up)
	{
		Vector3 direction = target - position;
		MP_ASSERT(MathOp::Abs(direction.x - std::numeric_limits<F32>::epsilon()) > 0.0f, "");
		MP_ASSERT(MathOp::Abs(direction.y - std::numeric_limits<F32>::epsilon()) > 0.0f, "");
		MP_ASSERT(MathOp::Abs(direction.z - std::numeric_limits<F32>::epsilon()) > 0.0f, "");

		SetViewDirection(position, direction, up);
	}

	void Camera::SetViewYXZ(Vector3 position, Vector3 rotation)
	{
		const float c3 = MathOp::Cos(rotation.z);
		const float s3 = MathOp::Sin(rotation.z);
		const float c2 = MathOp::Cos(rotation.x);
		const float s2 = MathOp::Sin(rotation.x);
		const float c1 = MathOp::Cos(rotation.y);
		const float s1 = MathOp::Sin(rotation.y);

		const Vector3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const Vector3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const Vector3 w{ (c2 * s1), (-s2), (c1 * c2) };

		m_viewMatrix = Matrix4{ 1.f };
		m_viewMatrix[0][0] = u.x;
		m_viewMatrix[1][0] = u.y;
		m_viewMatrix[2][0] = u.z;
		m_viewMatrix[0][1] = v.x;
		m_viewMatrix[1][1] = v.y;
		m_viewMatrix[2][1] = v.z;
		m_viewMatrix[0][2] = w.x;
		m_viewMatrix[1][2] = w.y;
		m_viewMatrix[2][2] = w.z;
		m_viewMatrix[3][0] = -MathOp::Dot(u, position);
		m_viewMatrix[3][1] = -MathOp::Dot(v, position);
		m_viewMatrix[3][2] = -MathOp::Dot(w, position);
	}

} // namespace Mapo
