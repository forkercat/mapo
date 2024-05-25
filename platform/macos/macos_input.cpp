//
// Created by Junhao Wang (@forkercat) on 5/1/24.
//

#include "core/core.h"

#include "engine/application.h"
#include "engine/input/input.h"

#ifdef MP_MACOS_BUILD
	#include <GLFW/glfw3.h>
#endif

namespace Mapo
{
#ifdef MP_MACOS_BUILD
	bool Input::IsKeyPressed(KeyCode keycode)
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, static_cast<I32>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsAnyModifierPressed()
	{
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool super = Input::IsKeyPressed(Key::LeftSuper) || Input::IsKeyPressed(Key::RightSuper);
		bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);
		return control || shift || super || alt;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<I32>(button));
		return state == GLFW_PRESS;
	}

	Vector2 Input::GetMousePosition()
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		F64 x{}, y{};
		glfwGetCursorPos(window, &x, &y);
		return { (F32)x, (F32)y };
	}

	F32 Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	F32 Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
#endif

} // namespace Mapo
