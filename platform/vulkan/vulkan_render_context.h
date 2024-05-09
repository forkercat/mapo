//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

#include "engine/render/render_context.h"

#include "platform/vulkan/vulkan_device.h"
#include "platform/vulkan/vulkan_renderer.h"
#include "platform/vulkan/vulkan_descriptors.h"
#include "platform/vulkan/vulkan_swapchain.h"

namespace Mapo
{
	class VulkanRenderContext : public RenderContext
	{
	public:
		VulkanRenderContext(Window& window);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void* Device() override { return m_device.get(); }
		virtual void* Renderer() override { return m_renderer.get(); }
		virtual void* DescriptorPool() override { return m_descriptorPool.get(); }

	private:
		Window& m_window;

		// Vulkan resources.
		UniqueRef<VulkanDevice> m_device{};
		UniqueRef<VulkanRenderer> m_renderer{};
		UniqueRef<VulkanDescriptorPool> m_descriptorPool{};
	};

} // namespace Mapo
