//
// Created by Junhao Wang (@forkercat) on 5/14/24.
//

#include "editor_camera.h"

#include "engine/input/input.h"

namespace Mapo
{
	EditorCamera::EditorCamera(F32 fov, F32 aspectRatio, F32 near, F32 far)
		: m_fov(fov), m_aspectRatio(aspectRatio), m_near(near), m_far(far),
		  Camera(GenerateProjectionMatrix(fov, aspectRatio, near, far))
	{
		UpdateView();
	}

	void EditorCamera::OnUpdate(Timestep dt)
	{
		Vector2 mousePosition = Input::GetMousePosition();
		Vector2 offset = (mousePosition - m_lastMousePosition) * 1.0f * dt.GetSeconds(); // TODO: USE dt!
		m_lastMousePosition = mousePosition;

		offset.y = -offset.y;

		bool modifier = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::LeftSuper);

		if (modifier)
		{
			if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
			{
				MouseMove({ 0, offset.y }); /// don't take the x value.
			}
		}
		else
		{
			// Mouse
			if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
			{
				MouseRotate(offset);

				// Key
				Vector2 moveDir{ 0.0f, 0.0f };

				if (Input::IsKeyPressed(Key::W))
				{
					moveDir.y -= 1.0f;
				}
				if (Input::IsKeyPressed(Key::S))
				{
					moveDir.y += 1.0f;
				}
				if (Input::IsKeyPressed(Key::A))
				{
					moveDir.x -= 1.0f;
				}
				if (Input::IsKeyPressed(Key::D))
				{
					moveDir.x += 1.0f;
				}

				if (MathOp::Length(moveDir) > std::numeric_limits<F32>::epsilon())
				{
					moveDir = MathOp::Normalize(moveDir);
					MouseMove(moveDir * dt.GetSeconds());
				}

				if (Input::IsKeyPressed(Key::Q))
				{
					MouseMoveVertical(-1.0f * dt.GetSeconds());
				}
				else if (Input::IsKeyPressed(Key::E))
				{
					MouseMoveVertical(1.0f * dt.GetSeconds());
				}
			}
			else if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
			{
				MousePan(offset);
			}
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		// dispatcher.Dispatch<MouseScrolledEvent>(MP_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
		// dispatcher.Dispatch<KeyPressedEvent>(MP_BIND_EVENT_FN(EditorCamera::OnKeyPressed));
	}

	/////////////////////////////////////////////////////////////////////////////////

	void EditorCamera::UpdateProjection()
	{
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
		m_projection = GenerateProjectionMatrix(m_fov, m_aspectRatio, m_near, m_far);
	}

	void EditorCamera::UpdateView()
	{
		// m_yaw = m_pitch = 0.0f; // Lock the camera position.

		glm::quat orientation = GetOrientation();
		m_view = glm::translate(Matrix4(1.0f), m_position) * glm::toMat4(orientation); // TRS
		m_view = glm::inverse(m_view);
	}

	/////////////////////////////////////////////////////////////////////////////////

	void EditorCamera::MousePan(const Vector2& offset)
	{
		Vector2 panSpeed = PanSpeed();
		m_position -= GetRightDirection() * offset.x * panSpeed.x;
		m_position -= GetUpDirection() * offset.y * panSpeed.y;
	}

	void EditorCamera::MouseRotate(const Vector2& offset)
	{
		float yawSign = GetUpDirection().y > 0 ? 1.0f : -1.0f;
		m_yaw += yawSign * offset.x * RotationSpeed();
		m_pitch -= offset.y * RotationSpeed();
	}

	void EditorCamera::MouseMove(const Vector2& offset)
	{
		m_position += GetForwardDirection() * offset.y * MoveSpeed();
		m_position += GetRightDirection() * offset.x * MoveSpeed();
	}

	void EditorCamera::MouseMoveVertical(F32 offsetY)
	{
		m_position += GetUpDirection() * offsetY * MoveSpeed();
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& event)
	{
		MouseMove({ 0, -event.GetOffsetY() * 0.05f });

		UpdateView();
		return false;
	}

	bool EditorCamera::OnKeyPressed(KeyPressedEvent& event)
	{
		//

		UpdateView();
		return false;
	}

	void EditorCamera::SetViewportSize(F32 width, F32 height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;

		UpdateProjection();
	}

	/////////////////////////////////////////////////////////////////////////////////

	Vector2 EditorCamera::PanSpeed() const
	{
		F32 MaxSpeed = 6.0f;

		// Calculate based on the viewport size
		F32 x = MathOp::Min(m_viewportWidth / 1000.0f, MaxSpeed);
		F32 factorX = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		F32 y = MathOp::Min(m_viewportHeight / 1000.0f, MaxSpeed);
		F32 factorY = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { factorX, factorY };
	}

	F32 EditorCamera::RotationSpeed() const
	{
		return 0.5f;
	}

	F32 EditorCamera::MoveSpeed() const
	{
		return 5.0f;
	}

} // namespace Mapo
