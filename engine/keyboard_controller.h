//
// Created by Junhao Wang (@forkercat) on 4/20/24.
//

#pragma once

#include "core/core.h"

#include "engine/game_object.h"
#include "engine/render/vulkan_window.h"

namespace Mapo
{
	class KeyboardController
	{
	public:
		struct KeyMappings
		{
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;

			int moveUp = GLFW_KEY_E;
			int moveDown = GLFW_KEY_Q;
			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
		};

		void MoveInPlaneXZ(GLFWwindow* window, F32 dt, GameObject& gameObject);

	public:
		KeyMappings keys{};
		F32 moveSpeed = 3.0f;
		F32 lookSpeed = 1.5f;
	};

} // namespace Mapo
