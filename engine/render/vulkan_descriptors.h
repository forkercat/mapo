//
// Created by Junhao Wang (@forkercat) on 4/22/24.
//

#pragma once

#include "core/core.h"

#include "vulkan_device.h"

namespace Mapo
{
	// Descriptor set layout
	class VulkanDescriptorSetLayout
	{
	public:
		class Builder
		{
		public:
			Builder(VulkanDevice& device)
				: m_device(device)
			{
			}

			Builder& AddBinding(U32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, U32 count = 1);
			UniqueRef<VulkanDescriptorSetLayout> Build() const;

		private:
			VulkanDevice& m_device;
			std::unordered_map<U32, VkDescriptorSetLayoutBinding> m_bindings{};
		};

	public:
		VulkanDescriptorSetLayout(VulkanDevice& device, std::unordered_map<U32, VkDescriptorSetLayoutBinding> bindings);
		~VulkanDescriptorSetLayout();

		VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
		VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }

	private:
		VulkanDevice& m_device;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::unordered_map<U32, VkDescriptorSetLayoutBinding> m_bindings;

		friend class VulkanDescriptorWriter;
	};

	// Descriptor pool
	class VulkanDescriptorPool
	{
	public:
		class Builder
		{
		public:
			Builder(VulkanDevice& device)
				: m_device(device)
			{
			}

			Builder& AddPoolSize(VkDescriptorType descriptorType, U32 descriptorCount);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& SetMaxSets(U32 count);
			UniqueRef<VulkanDescriptorPool> Build() const;

		private:
			VulkanDevice& m_device;
			std::vector<VkDescriptorPoolSize> m_poolSizes{};
			U32 m_maxSets = 1000;
			VkDescriptorPoolCreateFlags m_poolFlags = 0;
		};

	public:
		VulkanDescriptorPool(VulkanDevice& device, U32 maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
		~VulkanDescriptorPool();

		VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
		VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

		bool AllocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptorSet) const;
		void FreeDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets) const;
		void ResetPool();

	private:
		VulkanDevice& m_device;
		VkDescriptorPool m_descriptorPool;

		friend class VulkanDescriptorWriter;
	};

	// Descriptor writer
	class VulkanDescriptorWriter
	{
	public:
		VulkanDescriptorWriter(VulkanDescriptorSetLayout& descriptorSetLayout, VulkanDescriptorPool& descriptorPool);
		~VulkanDescriptorWriter() = default;

		VulkanDescriptorWriter(const VulkanDescriptorWriter&) = delete;
		VulkanDescriptorWriter& operator=(const VulkanDescriptorWriter&) = delete;

		VulkanDescriptorWriter& WriteBuffer(U32 binding, VkDescriptorBufferInfo* bufferInfo);
		VulkanDescriptorWriter& WriteImage(U32 binding, VkDescriptorImageInfo* imageInfo);

		bool Build(VkDescriptorSet& descriptorSet);
		void Overwrite(VkDescriptorSet& descriptorSet);

	private:
		VulkanDescriptorSetLayout& m_descriptorSetLayout;
		VulkanDescriptorPool& m_descriptorPool;
		std::vector<VkWriteDescriptorSet> m_writes;
	};

} // namespace Mapo
