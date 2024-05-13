//
// Created by Junhao Wang (@forkercat) on 4/20/24.
//

#pragma once

#include "core/core.h"

#include "engine/game_object.h"

class GLFWwindow;

namespace Mapo
{
	class KeyboardController
	{
	public:
		virtual ~KeyboardController() = default;

		void MoveInPlaneXZ(F32 dt, GameObject& gameObject);

	public:
		F32 moveSpeed = 3.0f;
		F32 lookSpeed = 1.5f;
	};

} // namespace Mapo
