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
		RotateScript(F32 speed = 30.0f)
			: Scriptable("RotateScript"), rotateSpeed(speed)
		{
		}

		virtual void OnCreate() override
		{
			MP_APP_INFO("RotateScript: OnCreate!");

			static F32 randomRotateSpeed = rotateSpeed;
			rotateSpeed = -randomRotateSpeed;
			randomRotateSpeed = rotateSpeed;
		}

		virtual void OnUpdateEditor(Timestep dt) override
		{
			auto& transform = GetComponent<TransformComponent>();
			transform.rotation.y += rotateSpeed * dt;

			if (transform.rotation.y > 360.0f)
			{
				transform.rotation.y -= 720.0f;
			}
			else if (transform.rotation.y < -360.0f)
			{
				transform.rotation.y += 720.0f;
			}
		}

	public:
		F32 rotateSpeed = 30.0f;
	};
} // namespace Mapo
