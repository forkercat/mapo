//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#include "vulkan_render_context.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Mapo
{
	VulkanRenderContext::VulkanRenderContext(GLFWwindow* windowHandle)
		: m_windowHandle(windowHandle)
	{
		MP_ASSERT(windowHandle, "Window handle is nullptr!");
	}

	void VulkanRenderContext::Init()
	{

	}

	void VulkanRenderContext::SwapBuffers()
	{

	}

} // namespace Mapo
