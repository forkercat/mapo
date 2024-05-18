//
// Created by Junhao Wang (@forkercat) on 5/14/24.
//

#pragma once

#include "engine/renderer/camera.h"

namespace Mapo
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic = 1
		};

		virtual ~SceneCamera() = default;

		SceneCamera();

		// Getters
		ProjectionType GetProjectionType() const { return m_projectionType; }

		F32 GetPerspectiveFov() const { return m_perspectiveFov; }
		F32 GetPerspectiveNearClip() const { return m_perspectiveNear; }
		F32 GetPerspectiveFarClip() const { return m_perspectiveFar; }

		F32 GetOrthographicSize() const { return m_orthographicSize; }
		F32 GetOrthographicNearClip() const { return m_orthographicNear; }
		F32 GetOrthographicFarClip() const { return m_orthographicFar; }

		// Setters
		void SetProjectionType();

		void SetOrthographic(F32 size, F32 aspectRatio, F32 near, F32 far);
		void SetPerspective(F32 vFov, F32 aspectRatio, F32 near, F32 far);
		void SetViewportSize(U32 width, U32 height);

		// TODO: REMOVE
		// void SetViewDirection(Vector3 position, Vector3 direction, Vector3 up = { 0.0f, -1.0f, 0.0f });
		// void SetViewTarget(Vector3 position, Vector3 target, Vector3 up = { 0.0f, -1.0f, 0.0f });
		// void SetViewYXZ(Vector3 position, Vector3 rotation);
		// const Matrix4& GetView() const { return m_viewMatrix; }

	private:
		void UpdateProjection();

	private:
		ProjectionType m_projectionType = ProjectionType::Perspective;

		// Perspective
		F32 m_perspectiveFov = MathOp::Radians(45.0f);
		F32 m_perspectiveNear = 0.01f;
		F32 m_perspectiveFar = 1000.0f;

		// Orthographic
		F32 m_orthographicSize = 10.0f;
		F32 m_orthographicNear = -1.0f;
		F32 m_orthographicFar = 1.0f;

		F32 m_aspectRatio = 1.0f; // TODO?
	};

} // namespace Mapo
