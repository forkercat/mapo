//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/game_object.h"
#include "engine/camera.h"

#include "engine/vulkan_device.h"
#include "engine/vulkan_pipeline.h"
#include "engine/vulkan_frame_info.h"

#include <vector>
#include <memory>

namespace mapo
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(VulkanFrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

	private:
		VulkanDevice& m_device;

		UniqueRef<VulkanPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};

} // namespace mapo
