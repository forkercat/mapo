//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#include "point_light_system.h"

#include "engine/renderer/vk_common.h"
#include "engine/renderer/render_context.h"
#include "engine/renderer/device.h"
#include "engine/renderer/pipeline.h"
#include "engine/renderer/frame_info.h"

namespace Mapo
{
	PointLightSystem::PointLightSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout)
		: m_device(RenderContext::GetDevice())
	{
		CreatePipelineLayout(globalDescriptorSetLayout);
		CreatePipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(m_device.GetDevice(), m_pipelineLayout, nullptr);
	}

	void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout)
	{
		// VkPushConstantRange pushConstantRange{};
		// pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		// pushConstantRange.offset = 0;
		// pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalDescriptorSetLayout };

		// This will be referenced throughout the program's lifetime.
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		pipelineLayoutInfo.setLayoutCount = static_cast<U32>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		VK_CHECK(vkCreatePipelineLayout(m_device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout));
	}

	void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
	{
		MP_ASSERT(m_pipelineLayout, "Could not create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_pipeline = MakeUnique<Pipeline>(
			"assets/shaders/point_light.vert.spv",
			"assets/shaders/point_light.frag.spv",
			pipelineConfig);
	}

	void PointLightSystem::Render(FrameInfo& frameInfo)
	{
		// Bind graphics pipeline.
		m_pipeline->Bind(frameInfo.commandBuffer);

		// Bind descriptor set.
		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}

} // namespace Mapo
