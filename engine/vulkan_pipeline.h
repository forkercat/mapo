//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#pragma once

#include "core/core.h"
#include "vulkan_device.h"

#include <vector>

namespace Mapo
{
	struct VulkanPipelineConfigInfo
	{
		VulkanPipelineConfigInfo(const VulkanPipelineConfigInfo&) = delete;
		VulkanPipelineConfigInfo& operator=(const VulkanPipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;

		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		U32 subpass = 0;
	};

	class VulkanPipeline
	{
	public:
		VulkanPipeline(VulkanDevice& device, const std::string& vertFilepath, const std::string& fragFilepath,
			const VulkanPipelineConfigInfo& configInfo);
		~VulkanPipeline();

		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer);

		static void DefaultPipelineConfigInfo(VulkanPipelineConfigInfo& configInfo);

	private:
		void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath,
			const VulkanPipelineConfigInfo& configInfo);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* pShaderModule);

		static std::vector<char> ReadFile(const std::string& filepath);

	private:
		VulkanDevice& m_device;
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertShaderModule;
		VkShaderModule m_fragShaderModule;
	};

} // namespace Mapo
