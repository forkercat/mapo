//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "vulkan_window.h"
#include "engine/model.h"

#include "vulkan_device.h"
#include "vulkan_swapchain.h"

#include <vector>
#include <memory>

namespace Mapo
{
	// Renderer class that manages swapchain and command buffers.
	class VulkanRenderer
	{
	public:
		VulkanRenderer(VulkanWindow& window, VulkanDevice& device);
		~VulkanRenderer();

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		// Public getter.
		VkRenderPass GetSwapchainRenderPass() const { return m_swapchain->GetRenderPass(); }
		F32 GetAspectRatio() const { return m_swapchain->GetExtentAspectRatio(); }
		U32 GetSwapchainImageCount() const { return m_swapchain->GetImageCount(); }
		bool IsFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer GetCurrentCommandBuffer() const
		{
			ASSERT(IsFrameInProgress(), "Could not get command buffer when frame is not in progress!");
			return m_commandBuffers[m_currentFrameIndex];
		}

		U32 GetCurrentFrameIndex() const
		{
			ASSERT(IsFrameInProgress(), "Could not get current frame index when frame is not in progress!");
			return m_currentFrameIndex;
		}

		// Functions to render.
		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapchainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapchainRenderPass(VkCommandBuffer commandBuffer);

	private:
		// Functions to create Vulkan resources.
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapchain();

	private:
		VulkanWindow& m_window;
		VulkanDevice& m_device;

		UniqueRef<VulkanSwapchain> m_swapchain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		U32 m_currentImageIndex = 0;
		U32 m_currentFrameIndex = 0;
		bool m_isFrameStarted = false;
	};

} // namespace Mapo
