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
		RotateScript()
			: Scriptable("RotateScript")
		{
		}

		virtual void OnCreate() override
		{
			MP_APP_INFO("RotateScript: OnCreate!");
		}

		virtual void OnUpdateEditor(Timestep dt) override
		{
			const F32 rotateSpeed = 30.0f;

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
	};
} // namespace Mapo
