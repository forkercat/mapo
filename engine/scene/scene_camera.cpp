//
// Created by Junhao Wang (@forkercat) on 5/14/24.
//

#include "scene_camera.h"

namespace Mapo
{
	SceneCamera::SceneCamera()
	{
		UpdateProjection();
	}

	void SceneCamera::SetPerspective(F32 vFov, F32 aspectRatio, F32 near, F32 far)
	{
		m_projectionType = ProjectionType::Perspective;
		m_perspectiveFov = vFov;
		m_perspectiveNear = near;
		m_perspectiveFar = far;

		m_aspectRatio = aspectRatio;

		UpdateProjection();
	}

	void SceneCamera::SetOrthographic(F32 size, F32 aspectRatio, F32 near, F32 far)
	{
		m_projectionType = ProjectionType::Orthographic;
		m_orthographicSize = size;
		m_orthographicNear = near;
		m_orthographicFar = far;

		m_aspectRatio = aspectRatio;

		UpdateProjection();
	}

	void SceneCamera::SetViewportSize(U32 width, U32 height)
	{
		m_aspectRatio = (F32)width / (F32)height;

		UpdateProjection();
	}

	void SceneCamera::UpdateProjection()
	{
		if (m_projectionType == ProjectionType::Perspective)
		{
			MP_ASSERT(MathOp::Abs(m_aspectRatio - std::numeric_limits<F32>::epsilon()) > 0.0f, "");

			const F32 tanHalfFovy = tan(m_perspectiveFov / 2.0f);
			const F32 farMinusNear = m_perspectiveFar - m_perspectiveNear;
			const F32 farTimesNear = m_perspectiveFar * m_perspectiveNear;

			m_projection = Matrix4{ 0.0f };
			m_projection[0][0] = 1.0f / (m_aspectRatio * tanHalfFovy);
			m_projection[1][1] = 1.0f / (tanHalfFovy);
			m_projection[2][2] = m_perspectiveFar / (farMinusNear);
			m_projection[2][3] = 1.0f;
			m_projection[3][2] = -(farTimesNear) / (farMinusNear);

			// m_projection = glm::perspective(m_perspectiveFov, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
		}
		else
		{
			const F32 left = m_orthographicSize * m_aspectRatio * -0.5f;
			const F32 right = m_orthographicSize * m_aspectRatio * 0.5f;
			const F32 bottom = m_orthographicSize * -0.5f;
			const F32 top = m_orthographicSize * 0.5f;
			const F32 farMinusNear = m_orthographicFar - m_orthographicNear;

			m_projection = Matrix4{ 1.0f };
			m_projection[0][0] = 2.0f / (right - left);
			m_projection[1][1] = 2.0f / (bottom - top);
			m_projection[2][2] = 1.0f / (farMinusNear);

			m_projection[3][0] = -(right + left) / (right - left);
			m_projection[3][1] = -(bottom + top) / (bottom - top);
			m_projection[3][2] = -m_orthographicNear / (farMinusNear);

			// m_projection = glm::ortho(left, right, bottom, top, m_orthographicNear, m_orthographicFar);
		}
	}

	/*
	void SceneCamera::SetViewDirection(Vector3 position, Vector3 direction, Vector3 up)
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

	void SceneCamera::SetViewTarget(Vector3 position, Vector3 target, Vector3 up)
	{
		Vector3 direction = target - position;
		MP_ASSERT(MathOp::Abs(direction.x - std::numeric_limits<F32>::epsilon()) > 0.0f, "");
		MP_ASSERT(MathOp::Abs(direction.y - std::numeric_limits<F32>::epsilon()) > 0.0f, "");
		MP_ASSERT(MathOp::Abs(direction.z - std::numeric_limits<F32>::epsilon()) > 0.0f, "");

		SetViewDirection(position, direction, up);
	}

	void SceneCamera::SetViewYXZ(Vector3 position, Vector3 rotation)
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
	*/

} // namespace Mapo
