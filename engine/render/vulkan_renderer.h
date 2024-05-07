//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/model.h"

namespace Mapo
{
	class Window;
	class VulkanDevice;
	class VulkanSwapchain;

	// Renderer class that manages swapchain and command buffers.
	class VulkanRenderer
	{
	public:
		VulkanRenderer(Window& window, VulkanDevice& device);
		~VulkanRenderer();

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		// Public getter.
		VkRenderPass GetSwapchainRenderPass() const;
		F32 GetAspectRatio() const;
		U32 GetSwapchainImageCount() const;
		bool IsFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer GetCurrentCommandBuffer() const
		{
			MP_ASSERT(IsFrameInProgress(), "Could not get command buffer when frame is not in progress!");
			return m_commandBuffers[m_currentFrameIndex];
		}

		U32 GetCurrentFrameIndex() const
		{
			MP_ASSERT(IsFrameInProgress(), "Could not get current frame index when frame is not in progress!");
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
		Window& m_window;
		VulkanDevice& m_device;

		UniqueRef<VulkanSwapchain> m_swapchain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		U32 m_currentImageIndex = 0;
		U32 m_currentFrameIndex = 0;
		bool m_isFrameStarted = false;
	};

} // namespace Mapo
