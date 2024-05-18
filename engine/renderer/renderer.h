//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include <vulkan/vulkan.h>

namespace Mapo
{
	class Device;
	class Swapchain;

	// Renderer class that manages swapchain and command buffers.
	class Renderer final
	{
	public:
		virtual ~Renderer();

		Renderer();
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		// Public getter.
		VkRenderPass GetRenderPass() const;
		F32			 GetAspectRatio() const;
		U32			 GetImageCount() const;
		bool		 IsFrameInProgress() const;
		U32			 GetSwapchainWidth() const;
		U32			 GetSwapchainHeight() const;

		VkCommandBuffer GetCurrentCommandBuffer()
		{
			MP_ASSERT(IsFrameInProgress(), "Could not get command buffer when frame is not in progress!");
			return m_commandBuffers[m_currentFrameIndex];
		}

		U32 GetCurrentFrameIndex()
		{
			MP_ASSERT(IsFrameInProgress(), "Could not get current frame index when frame is not in progress!");
			return m_currentFrameIndex;
		}

		// Functions to render.
		VkCommandBuffer BeginFrame();
		void			EndFrame();
		void			BeginRenderPass();
		void			EndRenderPass();

	private:
		// Functions to create Vulkan resources.
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapchain();

	private:
		Device&						 m_device;
		UniqueRef<Swapchain>		 m_swapchain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		U32	 m_currentImageIndex = 0;
		U32	 m_currentFrameIndex = 0;
		bool m_isFrameStarted = false;
	};

} // namespace Mapo
