//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/camera.h"
#include "engine/renderer/device.h"
#include "engine/renderer/pipeline.h"
#include "engine/renderer/frame_info.h"
#include "engine/game_object.h"

#include <vector>
#include <memory>

namespace Mapo
{
	class PointLightSystem
	{
	public:
		PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void Render(FrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

	private:
		Device& m_device;

		UniqueRef<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};

} // namespace Mapo
