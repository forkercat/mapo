//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#pragma once

#include "core/core.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mapo
{
	class Device;

	struct PipelineConfigInfo
	{
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription>   bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		VkPipelineViewportStateCreateInfo	   viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo   multisampleInfo;
		VkPipelineColorBlendAttachmentState	   colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo	   colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
		std::vector<VkDynamicState>			   dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo	   dynamicStateInfo;

		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass	 renderPass = nullptr;
		U32				 subpass = 0;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline();

		Pipeline(const std::string& vertFilepath, const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer);

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* pShaderModule);

		static std::vector<char> ReadFile(const std::string& filepath);

	private:
		Device&		   m_device;
		VkPipeline	   m_graphicsPipeline;
		VkShaderModule m_vertShaderModule;
		VkShaderModule m_fragShaderModule;
	};

} // namespace Mapo
