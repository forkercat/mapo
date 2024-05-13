//
// Created by Junhao Wang (@forkercat) on 4/20/24.
//

#include "keyboard_controller.h"

#include "engine/game_object.h"
#include "engine/component.h"

#include "engine/input/input.h"

namespace Mapo
{
	void KeyboardController::MoveInPlaneXZ(F32 dt, GameObject& gameObject)
	{
		Vector3 rotate{ 0.0f };

		if (Input::IsKeyPressed(Key::Right))
			rotate.y += 1.0f;
		if (Input::IsKeyPressed(Key::Left))
			rotate.y -= 1.0f;
		if (Input::IsKeyPressed(Key::Up))
			rotate.x += 1.0f;
		if (Input::IsKeyPressed(Key::Down))
			rotate.x -= 1.0f;

		auto& transform = gameObject.GetComponent<TransformComponent>();

		if (MathOp::Dot(rotate, rotate) > std::numeric_limits<F32>::epsilon())
		{
			transform.rotation += lookSpeed * dt * MathOp::Normalize(rotate);
		}

		// Limit pitch values between about +/- 85ish degrees.
		transform.rotation.x = MathOp::Clamp(transform.rotation.x, -1.5f, 1.5f);
		transform.rotation.y = glm::mod(transform.rotation.y, GLM_2_PI);

		float yaw = transform.rotation.y;
		const Vector3 forwardDir{ MathOp::Sin(yaw), 0.0f, MathOp::Cos(yaw) };
		const Vector3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
		const Vector3 upDir{ 0.0f, -1.0f, 0.0f };

		Vector3 moveDir{ 0.0f };
		if (Input::IsKeyPressed(Key::W))
			moveDir += forwardDir;
		if (Input::IsKeyPressed(Key::S))
			moveDir -= forwardDir;

		if (Input::IsKeyPressed(Key::D))
			moveDir += rightDir;
		if (Input::IsKeyPressed(Key::A))
			moveDir -= rightDir;

		if (Input::IsKeyPressed(Key::E))
			moveDir += upDir;
		if (Input::IsKeyPressed(Key::Q))
			moveDir -= upDir;

		if (MathOp::Dot(moveDir, moveDir) > std::numeric_limits<F32>::epsilon())
		{
			transform.translation += moveSpeed * dt * MathOp::Normalize(moveDir);
		}
	}

} // namespace Mapo
