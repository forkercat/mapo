//
// Created by Junhao Wang (@forkercat) on 4/22/24.
//

#include "descriptors.h"

#include "engine/renderer/vk_common.h"
#include "engine/renderer/render_context.h"
#include "engine/renderer/device.h"

namespace Mapo
{
	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor set layout builder
	/////////////////////////////////////////////////////////////////////////////////
	DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(
		U32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, U32 count)
	{
		MP_ASSERT(m_bindings.count(binding) == 0, "Failed to add binding to builder. The binding already exists!");

		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		m_bindings[binding] = layoutBinding;

		return *this;
	}

	UniqueRef<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
	{
		return MakeUnique<DescriptorSetLayout>(m_bindings);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor set layout
	/////////////////////////////////////////////////////////////////////////////////

	DescriptorSetLayout::DescriptorSetLayout(
		std::unordered_map<U32, VkDescriptorSetLayoutBinding> bindings)
		: m_device(RenderContext::GetDevice()), m_bindings(bindings)
	{
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};

		for (const auto& kv : bindings)
		{
			layoutBindings.push_back(kv.second);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<U32>(layoutBindings.size());
		layoutInfo.pBindings = layoutBindings.data();

		VkResult result = vkCreateDescriptorSetLayout(
			m_device.GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout);
		MP_ASSERT_EQ(result, VK_SUCCESS, "Failed to create descriptor set layout!");
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_device.GetDevice(), m_descriptorSetLayout, nullptr);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor pool builder
	/////////////////////////////////////////////////////////////////////////////////

	DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, U32 descriptorCount)
	{
		m_poolSizes.push_back({ descriptorType, descriptorCount });
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
	{
		m_poolFlags = flags;
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(U32 count)
	{
		m_maxSets = count;
		return *this;
	}

	UniqueRef<DescriptorPool> DescriptorPool::Builder::Build() const
	{
		return MakeUnique<DescriptorPool>(m_maxSets, m_poolFlags, m_poolSizes);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor pool
	/////////////////////////////////////////////////////////////////////////////////

	DescriptorPool::DescriptorPool(
		U32 maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
		: m_device(RenderContext::GetDevice())
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<U32>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		VK_CHECK(vkCreateDescriptorPool(m_device.GetDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool));
	}

	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(m_device.GetDevice(), m_descriptorPool, nullptr);
	}

	bool DescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptorSet) const
	{
		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = m_descriptorPool;
		allocateInfo.pSetLayouts = &descriptorSetLayout;
		allocateInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case and builds a new pool whenever an old pool fills up.
		VkResult result = vkAllocateDescriptorSets(m_device.GetDevice(), &allocateInfo, &descriptorSet);
		VK_CHECK(result);
		return result == VK_SUCCESS;
	}

	void DescriptorPool::FreeDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets) const
	{
		vkFreeDescriptorSets(m_device.GetDevice(), m_descriptorPool, static_cast<U32>(descriptorSets.size()), descriptorSets.data());
	}

	void DescriptorPool::ResetPool()
	{
		vkResetDescriptorPool(m_device.GetDevice(), m_descriptorPool, 0);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor writer
	/////////////////////////////////////////////////////////////////////////////////

	DescriptorWriter::DescriptorWriter(DescriptorSetLayout& descriptorSetLayout, DescriptorPool& descriptorPool)
		: m_descriptorSetLayout(descriptorSetLayout), m_descriptorPool(descriptorPool)
	{
	}

	DescriptorWriter& DescriptorWriter::WriteBuffer(U32 binding, VkDescriptorBufferInfo* bufferInfo)
	{
		MP_ASSERT(m_descriptorSetLayout.m_bindings.count(binding) == 1, "Layout does not contain specified binding!");

		VkDescriptorSetLayoutBinding& bindingDescription = m_descriptorSetLayout.m_bindings[binding];
		MP_ASSERT(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple!");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;
		m_writes.push_back(write);

		return *this;
	}

	DescriptorWriter& DescriptorWriter::WriteImage(U32 binding, VkDescriptorImageInfo* imageInfo)
	{
		MP_ASSERT(m_descriptorSetLayout.m_bindings.count(binding) == 1, "Layout does not contain specified binding!");

		VkDescriptorSetLayoutBinding& bindingDescription = m_descriptorSetLayout.m_bindings[binding];
		MP_ASSERT(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple!");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;
		m_writes.push_back(write);

		return *this;
	}

	bool DescriptorWriter::Build(VkDescriptorSet& descriptorSet)
	{
		bool success = m_descriptorPool.AllocateDescriptorSet(m_descriptorSetLayout.GetDescriptorSetLayout(), descriptorSet);
		if (!success)
		{
			return false;
		}

		Overwrite(descriptorSet);
		return true;
	}

	void DescriptorWriter::Overwrite(VkDescriptorSet& descriptorSet)
	{
		for (VkWriteDescriptorSet& write : m_writes)
		{
			write.dstSet = descriptorSet;
		}

		vkUpdateDescriptorSets(m_descriptorPool.m_device.GetDevice(), m_writes.size(), m_writes.data(), 0, nullptr);
	}

} // namespace Mapo
