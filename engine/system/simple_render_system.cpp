//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#include "simple_render_system.h"

#include "engine/scene/component.h"
#include "engine/model.h"

#include "engine/renderer/vk_common.h"
#include "engine/renderer/render_context.h"
#include "engine/renderer/device.h"
#include "engine/renderer/pipeline.h"
#include "engine/renderer/frame_info.h"

namespace Mapo
{
	struct SimplePushConstantData
	{
		Matrix4 modelMatrix{ 1.0f };
		Matrix4 normalMatrix{ 1.0f };
	};

	SimpleRenderSystem::SimpleRenderSystem(VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout)
		: m_device(RenderContext::GetDevice())
	{
		CreatePipelineLayout(globalDescriptorSetLayout);
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(m_device.GetDevice(), m_pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalDescriptorSetLayout };

		// This will be referenced throughout the program's lifetime.
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		pipelineLayoutInfo.setLayoutCount = static_cast<U32>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		VK_CHECK(vkCreatePipelineLayout(m_device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout));
	}

	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
	{
		MP_ASSERT(m_pipelineLayout, "Could not create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline = MakeUnique<Pipeline>(
			"assets/shaders/simple_shader.vert.spv",
			"assets/shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo)
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

		// Render objects.
		// For now the game object list has been filtered with TransformComponent, i.e., all components.
		for (GameObject& gameObject : frameInfo.gameObjects)
		{
			auto& transform = gameObject.GetComponent<TransformComponent>();

			if (gameObject.HasComponent<MeshComponent>())
			{
				auto& meshComponent = gameObject.GetComponent<MeshComponent>();

				if (!meshComponent.enabled)
				{
					continue;
				}

				SimplePushConstantData push{};
				push.modelMatrix = transform.GetTransformMatrix();
				push.normalMatrix = transform.GetNormalMatrix(); // glm automatically converts from mat4 to mat3

				vkCmdPushConstants(frameInfo.commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
					sizeof(SimplePushConstantData), &push);

				meshComponent.model->Bind(frameInfo.commandBuffer);
				meshComponent.model->Draw(frameInfo.commandBuffer);
			}
		}
	}

} // namespace Mapo
