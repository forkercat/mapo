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
	Renderer* Renderer::s_renderer = nullptr;

	Renderer::Renderer()
	{
		RecreateSwapchain();

		// For now, the command buffers are created once and will be reused in frames.
		CreateCommandBuffers();
	}

	Renderer::~Renderer()
	{
		FreeCommandBuffers();
	}

	void Renderer::Init()
	{
		MP_ASSERT(RenderContext::IsInitialized(), "Render context is not initialized!");
		MP_ASSERT(s_renderer == nullptr, "The renderer has already been initialized. You cannot initialize it twice!");

		s_renderer = MP_NEW(Renderer, StdAllocator::Get())();
	}

	void Renderer::Release()
	{
		MP_ASSERT(s_renderer, "The renderer instance is nullptr!");
		MP_DELETE(s_renderer, StdAllocator::Get());
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Public Getters.
	/////////////////////////////////////////////////////////////////////////////////

	VkRenderPass Renderer::GetRenderPass()
	{
		return s_renderer->m_swapchain->GetRenderPass();
	}

	F32 Renderer::GetAspectRatio()
	{
		return s_renderer->m_swapchain->GetExtentAspectRatio();
	}

	U32 Renderer::GetImageCount()
	{
		return s_renderer->m_swapchain->GetImageCount();
	}

	bool Renderer::IsFrameInProgress()
	{
		return s_renderer->m_isFrameStarted;
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

		VkResult acquireResult = s_renderer->m_swapchain->AcquireNextImage(&s_renderer->m_currentImageIndex);

		if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			s_renderer->RecreateSwapchain();
			return nullptr;
		}

		if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
		{
			MP_ASSERT(false, "Failed to acquire next image!");
		}

		s_renderer->m_isFrameStarted = true;

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
		VkResult submitResult = s_renderer->m_swapchain->SubmitCommandBuffers(&commandBuffer, &s_renderer->m_currentImageIndex);

		Window& window = Application::Get().GetWindow();

		if (submitResult == VK_ERROR_OUT_OF_DATE_KHR || submitResult == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
		{
			window.ResetWindowResizedFlag();
			s_renderer->RecreateSwapchain();
		}
		else if (submitResult != VK_SUCCESS)
		{
			MP_ASSERT(false, "Failed to submit command buffer!");
		}

		// Currently renderer and swapchain manages separate frame indices, but they are always identical.
		s_renderer->m_isFrameStarted = false;
		s_renderer->m_currentFrameIndex = (s_renderer->m_currentFrameIndex + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::BeginRenderPass(VkCommandBuffer commandBuffer)
	{
		MP_ASSERT(s_renderer->m_isFrameStarted, "Could not begin render pass while frame is not in progress!");
		MP_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Could not begin render pass on command buffer from a different frame!");

		// Begin render pass.
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = s_renderer->m_swapchain->GetRenderPass();
		renderPassBeginInfo.framebuffer = s_renderer->m_swapchain->GetFramebuffer(s_renderer->m_currentImageIndex);

		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = s_renderer->m_swapchain->GetSwapchainExtent();

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
		viewport.width = static_cast<F32>(s_renderer->m_swapchain->GetSwapchainExtent().width);
		viewport.height = static_cast<F32>(s_renderer->m_swapchain->GetSwapchainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = s_renderer->m_swapchain->GetSwapchainExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void Renderer::EndRenderPass(VkCommandBuffer commandBuffer)
	{
		MP_ASSERT(s_renderer->m_isFrameStarted, "Could not end render pass while frame is not in progress!");
		MP_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Could not end render pass on command buffer from a different frame!");

		vkCmdEndRenderPass(commandBuffer);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Functions to create Vulkan resources.
	/////////////////////////////////////////////////////////////////////////////////

	void Renderer::CreateCommandBuffers()
	{
		m_commandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT); // 2

		Device& device = RenderContext::GetDevice();

		VkCommandBufferAllocateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		bufferInfo.commandPool = device.GetCommandPool();
		bufferInfo.commandBufferCount = static_cast<U32>(m_commandBuffers.size());

		VK_CHECK(vkAllocateCommandBuffers(device.GetDevice(), &bufferInfo, m_commandBuffers.data()));
	}

	void Renderer::FreeCommandBuffers()
	{
		Device& device = RenderContext::GetDevice();

		vkFreeCommandBuffers(device.GetDevice(), device.GetCommandPool(), static_cast<U32>(m_commandBuffers.size()),
			m_commandBuffers.data());
		m_commandBuffers.clear();
	}

	void Renderer::RecreateSwapchain()
	{
		Device& device = RenderContext::GetDevice();
		Window& window = Application::Get().GetWindow();

		VkExtent2D extent{ window.GetWidth(), window.GetHeight() };

		// Handles window minimization.
		while (extent.width == 0 || extent.height == 0)
		{
			extent = { window.GetWidth(), window.GetHeight() };
			window.GlfwWaitEvents();
		}

		// Need to wait for the current swapchain not being used.
		vkDeviceWaitIdle(device.GetDevice());

		if (m_swapchain == nullptr)
		{
			// Happens in initialization.
			m_swapchain = MakeUnique<Swapchain>(device, extent);
		}
		else
		{
			// Happens in swapchain recreation.
			Ref<Swapchain> oldSwapchain = std::move(m_swapchain);
			m_swapchain = MakeUnique<Swapchain>(device, extent, oldSwapchain);

			// Since we are not recreating the pipeline, we need to check if the swapchain render pass
			// is still compatible with the color or depth format defined in the pipeline render pass.
			if (!oldSwapchain->CompareSwapchainFormats(*m_swapchain.get()))
			{
				MP_ASSERT(false, "Failed to recreate swapchain. The swapchain image or depth format has changed!");
			}
		}
	}

} // namespace Mapo
