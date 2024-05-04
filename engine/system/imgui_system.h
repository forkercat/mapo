//
// Created by Junhao Wang (@forkercat) on 4/24/24.
//

#pragma once

#include "core/core.h"

#include "engine/render/vulkan_window.h"
#include "engine/render/vulkan_device.h"

namespace Mapo
{
	class ImGuiSystem
	{
	public:
		ImGuiSystem(VulkanWindow& window, VulkanDevice& device, VkRenderPass renderPass, U32 imageCount);
		~ImGuiSystem();

		ImGuiSystem(const ImGuiSystem&) = delete;
		ImGuiSystem& operator=(const ImGuiSystem&) = delete;

		void NewFrame();

		// This tells ImGui that we're done setting up the current frame,
		// then gets the draw data from ImGui and uses it to record to the command buffer.
		void Render(VkCommandBuffer commandBuffer);

		void Update();
		void RunExample();

	public:
		// Example states
		bool showDemoWindow = true;
		bool showAnotherWindow = false;

	private:
		VulkanDevice& m_device;

		// We haven't yet covered descriptor pools in the tutorial series,
		// so I'm just going to create one for just imgui and store it here for now.
		// maybe its preferred to have a separate descriptor pool for imgui anyway.
		VkDescriptorPool m_descriptorPool;
	};

} // namespace Mapo
