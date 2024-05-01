//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#pragma once

#include "core/core.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Mapo
{
	class VulkanWindow
	{
	public:
		VulkanWindow(U32 width, U32 height, std::string name);
		~VulkanWindow();

		VulkanWindow(const VulkanWindow&) = delete;
		VulkanWindow& operator=(const VulkanWindow&) = delete;

		bool ShouldClose() { return glfwWindowShouldClose(m_nativeWindow); }
		bool WasWindowResized() { return m_framebufferResized; }
		void ResetWindowResizedFlag() { m_framebufferResized = false; }
		GLFWwindow* GetNativeWindow() const { return m_nativeWindow; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		VkExtent2D GetExtent() { return { static_cast<U32>(m_width), static_cast<U32>(m_height) }; }
		U32 GetWidth() { return m_width; }
		U32 GetHeight() { return m_height; }

	private:
		void InitWindow();

		static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

	private:
		U32 m_width;
		U32 m_height;
		bool m_framebufferResized = false;

		std::string m_windowName;
		GLFWwindow* m_nativeWindow;
	};

} // namespace Mapo
