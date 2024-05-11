//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/model.h"

namespace Mapo
{
	class Swapchain;

	// Renderer class that manages swapchain and command buffers.
	class Renderer final
	{
	public:
		virtual ~Renderer();

		static void Init();
		static bool IsInitialized() { return s_renderer != nullptr; }
		static void Release();

		// Public getter.
		static VkRenderPass GetRenderPass();
		static F32			GetAspectRatio();
		static U32			GetImageCount();
		static bool			IsFrameInProgress();

		static VkCommandBuffer GetCurrentCommandBuffer()
		{
			MP_ASSERT(IsFrameInProgress(), "Could not get command buffer when frame is not in progress!");
			return s_renderer->m_commandBuffers[s_renderer->m_currentFrameIndex];
		}

		static U32 GetCurrentFrameIndex()
		{
			MP_ASSERT(IsFrameInProgress(), "Could not get current frame index when frame is not in progress!");
			return s_renderer->m_currentFrameIndex;
		}

		// Functions to render.
		static VkCommandBuffer BeginFrame();
		static void			   EndFrame();
		static void			   BeginRenderPass(VkCommandBuffer commandBuffer);
		static void			   EndRenderPass(VkCommandBuffer commandBuffer);

	private:
		Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		// Functions to create Vulkan resources.
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapchain();

	private:
		UniqueRef<Swapchain>		 m_swapchain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		U32	 m_currentImageIndex = 0;
		U32	 m_currentFrameIndex = 0;
		bool m_isFrameStarted = false;

		static Renderer* s_renderer;
	};

} // namespace Mapo
