//
// Created by Junhao Wang (@forkercat) on 5/6/24.
//

#include "macos_window.h"

#include "core/logging.h"
#include "core/uassert.h"

#include "engine/renderer/vk_common.h"

#include "engine/event/application_event.h"
#include "engine/event/key_event.h"
#include "engine/event/mouse_event.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Mapo
{
	MacosWindow::MacosWindow(const WindowProps& props)
		: Window()
	{
		Init(props);
	}

	MacosWindow::~MacosWindow()
	{
		Shutdown();
	}

	void MacosWindow::Init(const WindowProps& props)
	{
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;

		int success = glfwInit();
		MP_ASSERT(success, "Failed to initialize GLFW!");

		// Set up configurations.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
		MP_ASSERT(m_window, "GLFW window creation failed. m_window is nullptr!");

		// Set up callbacks.
		glfwSetErrorCallback(ErrorCallback);
		glfwSetWindowSizeCallback(m_window, WindowResizeCallback);
		glfwSetWindowCloseCallback(m_window, WindowCloseCallback);
		glfwSetKeyCallback(m_window, KeyEventCallback);
		glfwSetCharCallback(m_window, CharEventCallback);
		glfwSetMouseButtonCallback(m_window, MouseEventCallback);
		glfwSetScrollCallback(m_window, ScrollEventCallback);
		glfwSetCursorPosCallback(m_window, CursorEventCallback);

		glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);

		// Set window data.
		glfwSetWindowUserPointer(m_window, &m_data);
	}

	void MacosWindow::Shutdown()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void MacosWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	void MacosWindow::WindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data.width = width;
		data.height = height;

		// Publish an event.
		WindowResizeEvent event(width, height);
		data.eventCallback(event); // This is Application::OnEvent(event)
	}

	void MacosWindow::WindowCloseCallback(GLFWwindow* window)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		WindowCloseEvent event;
		data.eventCallback(event);
	}

	void MacosWindow::KeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.eventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.eventCallback(event);
				break;
			}
		}
	}

	void MacosWindow::CharEventCallback(GLFWwindow* window, unsigned int keycode)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		KeyTypedEvent event(static_cast<KeyCode>(keycode));
		data.eventCallback(event);
	}

	void MacosWindow::MouseEventCallback(GLFWwindow* window, int button, int action, int mods)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.eventCallback(event);
				break;
			}
		}
	}

	void MacosWindow::ScrollEventCallback(GLFWwindow* window, double offsetX, double offsetY)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseScrolledEvent event((F32)offsetX, (F32)offsetY);
		data.eventCallback(event);
	}

	void MacosWindow::CursorEventCallback(GLFWwindow* window, double posX, double posY)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseMovedEvent event((F32)posX, (F32)posY);
		data.eventCallback(event);
	}

	void MacosWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowData* pWindowData = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		pWindowData->framebufferResized = true;
		pWindowData->width = width;
		pWindowData->height = height;
	}

	void MacosWindow::ErrorCallback(int error, const char* description)
	{
		MP_ERROR("GLFW Error (code: %d): %s", error, description);
	}

	void MacosWindow::SetEventCallback(const EventCallbackFn& callback)
	{
		m_data.eventCallback = callback;
	}

	void MacosWindow::CreateWindowSurface(void* instance, void* surface)
	{
		VkInstance*	  pInstance = (VkInstance*)instance;
		VkSurfaceKHR* pSurface = (VkSurfaceKHR*)surface;

		VK_CHECK(glfwCreateWindowSurface(*pInstance, m_window, nullptr, pSurface));
	}

	void MacosWindow::GlfwWaitEvents()
	{
		glfwWaitEvents();
	}

	const char** MacosWindow::GlfwGetRequiredExtensions(U32* count)
	{
		return glfwGetRequiredInstanceExtensions(count);
	}

} // namespace Mapo
