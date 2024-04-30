//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#include "vulkan_window.h"

namespace mapo
{
	VulkanWindow::VulkanWindow(U32 width, U32 height, std::string name)
		: m_width(width), m_height(height), m_windowName(name)
	{
		InitWindow();
	}

	VulkanWindow::~VulkanWindow()
	{
		glfwDestroyWindow(m_nativeWindow);
		glfwTerminate();
	}

	void VulkanWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_nativeWindow = glfwCreateWindow((int)m_width, (int)m_height, m_windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_nativeWindow, this);
		glfwSetFramebufferSizeCallback(m_nativeWindow, FrameBufferResizeCallback);

		ASSERT(m_nativeWindow, "Failed to create glfw window!");
	}

	void VulkanWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		VkResult result = glfwCreateWindowSurface(instance, m_nativeWindow, nullptr, surface);
		ASSERT_EQ(result, VK_SUCCESS, "Failed to create a window surface for Vulkan!");
	}

	void VulkanWindow::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		VulkanWindow* pWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));
		pWindow->m_framebufferResized = true;
		pWindow->m_width = width;
		pWindow->m_height = height;
	}

} // namespace mapo
