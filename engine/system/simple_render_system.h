//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#pragma once

#include "core/core.h"

#include "engine/scene/game_object.h"
#include "engine/renderer/camera.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Mapo
{
	class Device;
	class Pipeline;
	class FrameInfo;

	class SimpleRenderSystem
	{
	public:
		virtual ~SimpleRenderSystem();

		SimpleRenderSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout);

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

	private:
		Device& m_device;

		UniqueRef<Pipeline> m_pipeline;
		VkPipelineLayout	m_pipelineLayout;
	};

} // namespace Mapo
