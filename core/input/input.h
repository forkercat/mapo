//
// Created by Junhao Wang (@forkercat) on 5/1/24.
//

#pragma once

#include "core/typedefs.h"
#include "core/input/key_codes.h"
#include "core/input/mouse_codes.h"

#include "core/math.h"

namespace Mapo
{
	// Implemented by different platform!
	class Input
	{
	public:
		virtual ~Input() = default;

		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseCode button);

		static Vector2 GetMousePosition();
		static F32 GetMouseX();
		static F32 GetMouseY();
	};

} // namespace Mapo
