//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#include "renderer.h"

#include "engine/window.h"
#include "engine/application.h"

#include "engine/renderer/vk_common.h"
#include "engine/renderer/render_context.h"
#include "engine/renderer/device.h"
#include "engine/renderer/swapchain.h"

namespace Mapo
{
	Renderer::Renderer()
		: m_device(RenderContext::GetDevice())
	{
		RecreateSwapchain();

		// For now, the command buffers are created once and will be reused in frames.
		CreateCommandBuffers();
	}

	Renderer::~Renderer()
	{
		FreeCommandBuffers();
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Public Getters.
	/////////////////////////////////////////////////////////////////////////////////

	VkRenderPass Renderer::GetRenderPass()
	{
		return m_swapchain->GetRenderPass();
	}

	F32 Renderer::GetAspectRatio()
	{
		return m_swapchain->GetExtentAspectRatio();
	}

	U32 Renderer::GetImageCount()
	{
		return m_swapchain->GetImageCount();
	}

	bool Renderer::IsFrameInProgress()
	{
		return m_isFrameStarted;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Functions to render.
	/////////////////////////////////////////////////////////////////////////////////

	VkCommandBuffer Renderer::BeginFrame()
	{
		MP_ASSERT(!IsFrameInProgress(), "Could not call BeginFrame while already in frame progress!");

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
			MP_ASSERT(false, "Failed to acquire next image!");
		}

		m_isFrameStarted = true;

		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer(); // based on m_currentFrameIndex

		// Begin command buffer.
		VkCommandBufferBeginInfo bufferBeginInfo{};
		bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo));

		return commandBuffer;
	}

	void Renderer::EndFrame()
	{
		MP_ASSERT(IsFrameInProgress(), "Could not call EndFrame while frame is not in progress!");

		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

		VK_CHECK(vkEndCommandBuffer(commandBuffer));

		// Submit command buffer.
		VkResult submitResult = m_swapchain->SubmitCommandBuffers(&commandBuffer, &m_currentImageIndex);

		Window& window = Application::Get().GetWindow();

		if (submitResult == VK_ERROR_OUT_OF_DATE_KHR || submitResult == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
		{
			window.ResetWindowResizedFlag();
			RecreateSwapchain();
		}
		else if (submitResult != VK_SUCCESS)
		{
			MP_ASSERT(false, "Failed to submit command buffer!");
		}

		// Currently renderer and swapchain manages separate frame indices, but they are always identical.
		m_isFrameStarted = false;
		m_currentFrameIndex = (m_currentFrameIndex + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::BeginRenderPass()
	{
		MP_ASSERT(m_isFrameStarted, "Could not begin render pass while frame is not in progress!");
		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

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

	void Renderer::EndRenderPass()
	{
		MP_ASSERT(m_isFrameStarted, "Could not end render pass while frame is not in progress!");
		vkCmdEndRenderPass(GetCurrentCommandBuffer());
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Functions to create Vulkan resources.
	/////////////////////////////////////////////////////////////////////////////////

	void Renderer::CreateCommandBuffers()
	{
		m_commandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT); // 2

		VkCommandBufferAllocateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		bufferInfo.commandPool = m_device.GetCommandPool();
		bufferInfo.commandBufferCount = static_cast<U32>(m_commandBuffers.size());

		VK_CHECK(vkAllocateCommandBuffers(m_device.GetDevice(), &bufferInfo, m_commandBuffers.data()));
	}

	void Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_device.GetDevice(), m_device.GetCommandPool(), static_cast<U32>(m_commandBuffers.size()),
			m_commandBuffers.data());
		m_commandBuffers.clear();
	}

	void Renderer::RecreateSwapchain()
	{
		Window& window = Application::Get().GetWindow();

		VkExtent2D extent{ window.GetWidth(), window.GetHeight() };

		// Handles window minimization.
		while (extent.width == 0 || extent.height == 0)
		{
			extent = { window.GetWidth(), window.GetHeight() };
			window.GlfwWaitEvents();
		}

		// Need to wait for the current swapchain not being used.
		vkDeviceWaitIdle(m_device.GetDevice());

		if (m_swapchain == nullptr)
		{
			// Happens in initialization.
			m_swapchain = MakeUnique<Swapchain>(extent);
		}
		else
		{
			// Happens in swapchain recreation.
			Ref<Swapchain> oldSwapchain = std::move(m_swapchain);
			m_swapchain = MakeUnique<Swapchain>(extent, oldSwapchain);

			// Since we are not recreating the pipeline, we need to check if the swapchain render pass
			// is still compatible with the color or depth format defined in the pipeline render pass.
			if (!oldSwapchain->CompareSwapchainFormats(*m_swapchain.get()))
			{
				MP_ASSERT(false, "Failed to recreate swapchain. The swapchain image or depth format has changed!");
			}
		}
	}

} // namespace Mapo
