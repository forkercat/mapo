//
// Created by Junhao Wang (@forkercat) on 5/17/24.
//

#pragma once

#include "engine/scene/scriptable.h"
#include "engine/scene/component.h"

namespace Mapo
{
	class RotateScript : public Scriptable
	{
	public:
		virtual void OnCreate() override
		{
			MP_APP_INFO("RotateScript: OnCreate!");
		}

		virtual void OnUpdateEditor(Timestep dt) override
		{
			const F32 rotateSpeed = 30.0f;

			auto& transform = GetComponent<TransformComponent>();
			transform.rotation.y += MathOp::Radians(rotateSpeed * dt);
		}
	};
} // namespace Mapo
