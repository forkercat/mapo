//
// Created by Junhao Wang (@forkercat) on 5/6/24.
//

#include "macos_window.h"

#include "core/logging.h"
#include "core/uassert.h"
#include "engine/render/render_context.h"

// TODO: REMOVED!
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Mapo
{
	MacosWindow::MacosWindow(const WindowProps& props)
		: Window(), m_renderContext(nullptr)
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

		m_renderContext = RenderContext::Create(*this);
		m_renderContext->Init();

		// Set up callbacks.
		glfwSetErrorCallback(ErrorCallback);
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

	bool MacosWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_window) != 0;
	}

	void MacosWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowData* pWindowData = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
		pWindowData->m_framebufferResized = true;
		pWindowData->width = width;
		pWindowData->height = height;
	}

	void MacosWindow::ErrorCallback(int error, const char* description)
	{
		MP_ERROR("GLFW Error (code: %d): %s", error, description);
	}

	void MacosWindow::CreateWindowSurface(void* instance, void* surface)
	{
		VkInstance* pInstance = (VkInstance*)instance;
		VkSurfaceKHR* pSurface = (VkSurfaceKHR*)surface;

		VkResult result = glfwCreateWindowSurface(*pInstance, m_window, nullptr, pSurface);
		MP_ASSERT_EQ(result, VK_SUCCESS, "Failed to create a window surface for Vulkan!");
	}

	void MacosWindow::GlfwWaitEvents()
	{
		glfwWaitEvents();
	}

	const char** MacosWindow::GlfwGetRequiredExtensions(U32* count)
	{
		return glfwGetRequiredInstanceExtensions(count);
	}

	void* MacosWindow::Device()
	{
		return m_renderContext->Device();
	}

	void* MacosWindow::Renderer()
	{
		return m_renderContext->Renderer();
	}

	void* MacosWindow::DescriptorPool()
	{
		return m_renderContext->DescriptorPool();
	}

} // namespace Mapo
