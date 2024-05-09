//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#include "vulkan_render_context.h"

#include "engine/window.h"

#include <vulkan/vulkan.h>

namespace Mapo
{
	VulkanRenderContext::VulkanRenderContext(Window& window)
		: m_window(window)
	{
		m_device = MakeUnique<VulkanDevice>(window);
		m_renderer = MakeUnique<VulkanRenderer>(m_window, *m_device);

		m_descriptorPool =
			VulkanDescriptorPool::Builder(*m_device)
				// How many descriptor sets can be created from the pool.
				.SetMaxSets(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
				// How many descriptors of this type are available in the pool.
				.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
				.Build();
	}

	void VulkanRenderContext::Init()
	{
	}

	void VulkanRenderContext::SwapBuffers()
	{
	}

} // namespace Mapo
