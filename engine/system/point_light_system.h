//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/camera.h"
#include "engine/render/vulkan_device.h"
#include "engine/render/vulkan_pipeline.h"
#include "engine/render/vulkan_frame_info.h"
#include "engine/game_object.h"

#include <vector>
#include <memory>

namespace Mapo
{
	class PointLightSystem
	{
	public:
		PointLightSystem(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void Render(VulkanFrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

	private:
		VulkanDevice& m_device;

		UniqueRef<VulkanPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};

} // namespace Mapo
