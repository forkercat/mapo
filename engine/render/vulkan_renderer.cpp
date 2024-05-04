//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#include "vulkan_renderer.h"

namespace Mapo
{
	VulkanRenderer::VulkanRenderer(VulkanWindow& window, VulkanDevice& device)
		: m_window(window), m_device(device)
	{
		RecreateSwapchain();

		// For now, the command buffers are created once and will be reused in frames.
		CreateCommandBuffers();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		FreeCommandBuffers();
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Functions to render.
	/////////////////////////////////////////////////////////////////////////////////

	VkCommandBuffer VulkanRenderer::BeginFrame()
	{
		ASSERT(!m_isFrameStarted, "Could not call BeginFrame while already in frame progress!");

		// Needs to synchronize the below calls because on GPU they are executed asynchronously.
		// 1. Acquire an image from the swapchain.
		// 2. Execute commands that draw onto the acquired image.
		// 3. Present that image to the screen for presentation, returning it to the swapchain.
		// The function calls will return before the operations are actually finished and the order of execution is also undefined.

		VkResult acquireResult = m_swapchain->AcquireNextImage(&m_currentImageIndex);

		if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return nullptr;
		}

		if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
		{
			ASSERT(false, "Failed to acquire next image!");
		}

		m_isFrameStarted = true;

		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer(); // based on m_currentFrameIndex

		// Begin command buffer.
		VkCommandBufferBeginInfo bufferBeginInfo{};
		bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkResult beginResult = vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);
		ASSERT_EQ(beginResult, VK_SUCCESS, "Failed to begin recording command buffer!");

		return commandBuffer;
	}

	void VulkanRenderer::EndFrame()
	{
		ASSERT(m_isFrameStarted, "Could not call EndFrame while frame is not in progress!");

		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

		VkResult endResult = vkEndCommandBuffer(commandBuffer);
		ASSERT_EQ(endResult, VK_SUCCESS, "Failed to end command buffer!");

		// Submit command buffer.
		VkResult submitResult = m_swapchain->SubmitCommandBuffers(&commandBuffer, &m_currentImageIndex);

		if (submitResult == VK_ERROR_OUT_OF_DATE_KHR || submitResult == VK_SUBOPTIMAL_KHR || m_window.WasWindowResized())
		{
			m_window.ResetWindowResizedFlag();
			RecreateSwapchain();
		}
		else if (submitResult != VK_SUCCESS)
		{
			ASSERT(false, "Failed to submit command buffer!");
		}

		// Currently renderer and swapchain manages separate frame indices, but they are always identical.
		m_isFrameStarted = false;
		m_currentFrameIndex = (m_currentFrameIndex + 1) % VulkanSwapchain::MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::BeginSwapchainRenderPass(VkCommandBuffer commandBuffer)
	{
		ASSERT(m_isFrameStarted, "Could not begin render pass while frame is not in progress!");
		ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Could not begin render pass on command buffer from a different frame!");

		// Begin render pass.
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_swapchain->GetRenderPass();
		renderPassBeginInfo.framebuffer = m_swapchain->GetFramebuffer(m_currentImageIndex);

		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_swapchain->GetSwapchainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { { 0.01f, 0.01f, 0.01f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<U32>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Dynamic viewport and scissor
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<F32>(m_swapchain->GetSwapchainExtent().width);
		viewport.height = static_cast<F32>(m_swapchain->GetSwapchainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swapchain->GetSwapchainExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanRenderer::EndSwapchainRenderPass(VkCommandBuffer commandBuffer)
	{
		ASSERT(m_isFrameStarted, "Could not end render pass while frame is not in progress!");
		ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Could not end render pass on command buffer from a different frame!");

		vkCmdEndRenderPass(commandBuffer);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Functions to create Vulkan resources.
	/////////////////////////////////////////////////////////////////////////////////

	void VulkanRenderer::CreateCommandBuffers()
	{
		m_commandBuffers.resize(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT); // 2

		VkCommandBufferAllocateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		bufferInfo.commandPool = m_device.GetCommandPool();
		bufferInfo.commandBufferCount = static_cast<U32>(m_commandBuffers.size());

		VkResult result = vkAllocateCommandBuffers(m_device.GetDevice(), &bufferInfo, m_commandBuffers.data());
		ASSERT_EQ(result, VK_SUCCESS, "Failed to create command buffers!");
	}

	void VulkanRenderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_device.GetDevice(), m_device.GetCommandPool(), static_cast<U32>(m_commandBuffers.size()),
			m_commandBuffers.data());
		m_commandBuffers.clear();
	}

	void VulkanRenderer::RecreateSwapchain()
	{
		VkExtent2D extent = m_window.GetExtent();

		// Handles window minimization.
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_window.GetExtent();
			glfwWaitEvents();
		}

		// Need to wait for the current swapchain not being used.
		vkDeviceWaitIdle(m_device.GetDevice());

		if (m_swapchain == nullptr)
		{
			// Happens in initialization.
			m_swapchain = MakeUniqueRef<VulkanSwapchain>(m_device, extent);
		}
		else
		{
			// Happens in swapchain recreation.
			Ref<VulkanSwapchain> oldSwapchain = std::move(m_swapchain);
			m_swapchain = MakeUniqueRef<VulkanSwapchain>(m_device, extent, oldSwapchain);

			// Since we are not recreating the pipeline, we need to check if the swapchain render pass
			// is still compatible with the color or depth format defined in the pipeline render pass.
			if (!oldSwapchain->CompareSwapchainFormats(*m_swapchain.get()))
			{
				ASSERT(false, "Failed to recreate swapchain. The swapchain image or depth format has changed!");
			}
		}
	}

} // namespace Mapo
