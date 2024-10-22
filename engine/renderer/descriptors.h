//
// Created by Junhao Wang (@forkercat) on 4/22/24.
//

#pragma once

#include "core/core.h"

#include <vulkan/vulkan.h>

namespace Mapo
{
	class Device;

	// Descriptor set layout
	class DescriptorSetLayout
	{
	public:
		class Builder
		{
		public:
			Builder() = default;

			Builder& AddBinding(U32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, U32 count = 1);
			UniqueRef<DescriptorSetLayout> Build() const;

		private:
			std::unordered_map<U32, VkDescriptorSetLayoutBinding> m_bindings{};
		};

	public:
		virtual ~DescriptorSetLayout();

		DescriptorSetLayout(std::unordered_map<U32, VkDescriptorSetLayoutBinding> bindings);

		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }

	private:
		Device& m_device;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::unordered_map<U32, VkDescriptorSetLayoutBinding> m_bindings;

		friend class DescriptorWriter;
	};

	// Descriptor pool
	class DescriptorPool
	{
	public:
		class Builder
		{
		public:
			Builder() = default;

			Builder& AddPoolSize(VkDescriptorType descriptorType, U32 descriptorCount);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& SetMaxSets(U32 count);
			UniqueRef<DescriptorPool> Build() const;

		private:
			std::vector<VkDescriptorPoolSize> m_poolSizes{};
			U32 m_maxSets = 1000;
			VkDescriptorPoolCreateFlags m_poolFlags = 0;
		};

	public:
		virtual ~DescriptorPool();

		DescriptorPool(U32 maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);

		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		bool AllocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptorSet) const;
		void FreeDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets) const;
		void ResetPool();

	private:
		Device& m_device;
		VkDescriptorPool m_descriptorPool;

		friend class DescriptorWriter;
	};

	// Descriptor writer
	class DescriptorWriter
	{
	public:
		virtual ~DescriptorWriter() = default;

		DescriptorWriter(DescriptorSetLayout& descriptorSetLayout, DescriptorPool& descriptorPool);

		DescriptorWriter(const DescriptorWriter&) = delete;
		DescriptorWriter& operator=(const DescriptorWriter&) = delete;

		DescriptorWriter& WriteBuffer(U32 binding, VkDescriptorBufferInfo* bufferInfo);
		DescriptorWriter& WriteImage(U32 binding, VkDescriptorImageInfo* imageInfo);

		bool Build(VkDescriptorSet& descriptorSet);
		void Overwrite(VkDescriptorSet& descriptorSet);

	private:
		DescriptorSetLayout& m_descriptorSetLayout;
		DescriptorPool& m_descriptorPool;
		std::vector<VkWriteDescriptorSet> m_writes;
	};

} // namespace Mapo
