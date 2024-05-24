//
// Created by Junhao Wang (@forkercat) on 5/14/24.
//

#pragma once

#include "engine/renderer/camera.h"
#include "engine/event/event.h"
#include "engine/event/mouse_event.h"
#include "engine/event/key_event.h"

namespace Mapo
{
	class EditorCamera : public Camera
	{
	public:
		virtual ~EditorCamera() = default;

		EditorCamera(F32 fov, F32 aspectRatio, F32 near = 0.1f, F32 far = 1000.f);

		void OnUpdate(Timestep dt);
		void OnEvent(Event& event);

		// Matrix
		const Matrix4& GetViewMatrix() const { return m_view; }
		Matrix4&	   GetViewMatrix() { return m_view; }
		Matrix4		   GetViewProjectionMatrix() const { return m_projection * m_view; }

		// Direction
		Vector3 GetUpDirection() const { return glm::rotate(GetOrientation(), Vector3(0.0f, 1.0f, 0.0f)); }
		Vector3 GetRightDirection() const { return glm::rotate(GetOrientation(), Vector3(1.0f, 0.0f, 0.0f)); }
		Vector3 GetForwardDirection() const { return glm::rotate(GetOrientation(), Vector3(0.0f, 0.0f, 1.0f)); } // Point to the viewer by convention

		// Position (cannot be set)
		const Vector3& GetPosition() const { return m_position; }
		Vector3&	   GetPosition() { return m_position; }

		// Fov
		F32& GetFov() { return m_fov; }

		// Orientation
		glm::quat GetOrientation() const { return glm::quat(Vector3(-m_pitch, -m_yaw, 0.0f)); }

		F32 GetPitch() const { return m_pitch; }
		F32 GetYaw() const { return m_yaw; }

		void SetViewportSize(F32 width, F32 height);

		// Update matrix
		void UpdateProjection();
		void UpdateView();

	private:

		// Movement
		void MousePan(const Vector2& offset);
		void MouseRotate(const Vector2& offset);
		void MouseMove(const Vector2& offset);
		void MouseMoveVertical(F32 offsetY);

		bool OnMouseScroll(MouseScrolledEvent& event);
		bool OnKeyPressed(KeyPressedEvent& event);

		Vector2 PanSpeed() const;
		F32		RotationSpeed() const;
		F32		MoveSpeed() const;
		F32		ZoomSpeed() const;

		static Matrix4 GenerateProjectionMatrix(F32 fov, F32 aspectRatio, F32 near, F32 far)
		{
			Matrix4 matrix = glm::perspective(MathOp::Radians(fov), aspectRatio, near, far);
			matrix[1][1] *= -1; // flip for vulkan
			return matrix;
		}

	private:
		F32 m_fov = 45.0f;
		F32 m_aspectRatio = 1.6f;
		F32 m_near = 0.1f;
		F32 m_far = 1000.0f;

		F32 m_viewportWidth = 1280;
		F32 m_viewportHeight = 800;

		Vector3 m_position = { 0.0f, 0.0f, 8.0f };
		Vector2 m_lastMousePosition = { 0.0f, 0.0f };

		F32 m_pitch = 0.0f;
		F32 m_yaw = 0.0f;

		Matrix4 m_view{ 1.0f };
	};
} // namespace Mapo
