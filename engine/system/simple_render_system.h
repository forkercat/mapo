//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/game_object.h"
#include "engine/camera.h"

#include "engine/renderer/device.h"
#include "engine/renderer/pipeline.h"
#include "engine/renderer/frame_info.h"

#include <vector>
#include <memory>

namespace Mapo
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

	private:
		Device& m_device;

		UniqueRef<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};

} // namespace Mapo
