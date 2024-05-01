//
// Created by Junhao Wang (@forkercat) on 4/14/24.
//

#include "simple_render_system.h"

#include "engine/component.h"

namespace Mapo
{
	struct SimplePushConstantData
	{
		Matrix4 modelMatrix{ 1.0f };
		Matrix4 normalMatrix{ 1.0f };
	};

	SimpleRenderSystem::SimpleRenderSystem(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSetLayout)
		: m_device(device)
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

		VkResult result = vkCreatePipelineLayout(m_device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
		ASSERT_EQ(result, VK_SUCCESS, "Failed to create pipeline layout!");
	}

	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
	{
		ASSERT(m_pipelineLayout, "Could not create pipeline before pipeline layout!");

		VulkanPipelineConfigInfo pipelineConfig{};
		VulkanPipeline::DefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_pipeline =
			MakeUniqueRef<VulkanPipeline>(m_device, "assets/shaders/simple_shader.vert.spv", "assets/shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void SimpleRenderSystem::RenderGameObjects(VulkanFrameInfo& frameInfo)
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
				auto& mesh = gameObject.GetComponent<MeshComponent>();

				SimplePushConstantData push{};
				push.modelMatrix = transform.GetTransform();
				push.normalMatrix = transform.GetNormalMatrix(); // glm automatically converts from mat4 to mat3

				vkCmdPushConstants(frameInfo.commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
					sizeof(SimplePushConstantData), &push);

				mesh.model->Bind(frameInfo.commandBuffer);
				mesh.model->Draw(frameInfo.commandBuffer);
			}
		}
	}

} // namespace Mapo
