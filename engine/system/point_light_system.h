//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/camera.h"
#include "engine/game_object.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Mapo
{
	class Device;
	class Pipeline;
	class FrameInfo;

	class PointLightSystem
	{
	public:
		virtual ~PointLightSystem();

		PointLightSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout);

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void Render(FrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

	private:
		Device& m_device;

		UniqueRef<Pipeline> m_pipeline;
		VkPipelineLayout	m_pipelineLayout;
	};

} // namespace Mapo
