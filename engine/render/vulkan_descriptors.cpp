//
// Created by Junhao Wang (@forkercat) on 4/22/24.
//

#include "vulkan_descriptors.h"

namespace Mapo
{
	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor set layout builder
	/////////////////////////////////////////////////////////////////////////////////
	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::AddBinding(
		U32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, U32 count)
	{
		ASSERT(m_bindings.count(binding) == 0, "Failed to add binding to builder. The binding already exists!");

		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		m_bindings[binding] = layoutBinding;

		return *this;
	}

	UniqueRef<VulkanDescriptorSetLayout> VulkanDescriptorSetLayout::Builder::Build() const
	{
		return MakeUniqueRef<VulkanDescriptorSetLayout>(m_device, m_bindings);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor set layout
	/////////////////////////////////////////////////////////////////////////////////

	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
		VulkanDevice& device, std::unordered_map<U32, VkDescriptorSetLayoutBinding> bindings)
		: m_device(device), m_bindings(bindings)
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
		ASSERT_EQ(result, VK_SUCCESS, "Failed to create descriptor set layout!");
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_device.GetDevice(), m_descriptorSetLayout, nullptr);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor pool builder
	/////////////////////////////////////////////////////////////////////////////////

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, U32 descriptorCount)
	{
		m_poolSizes.push_back({ descriptorType, descriptorCount });
		return *this;
	}

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
	{
		m_poolFlags = flags;
		return *this;
	}

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::SetMaxSets(U32 count)
	{
		m_maxSets = count;
		return *this;
	}

	UniqueRef<VulkanDescriptorPool> VulkanDescriptorPool::Builder::Build() const
	{
		return MakeUniqueRef<VulkanDescriptorPool>(m_device, m_maxSets, m_poolFlags, m_poolSizes);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor pool
	/////////////////////////////////////////////////////////////////////////////////

	VulkanDescriptorPool::VulkanDescriptorPool(
		VulkanDevice& device, U32 maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
		: m_device(device)
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<U32>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		VkResult result = vkCreateDescriptorPool(m_device.GetDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool);
		ASSERT_EQ(result, VK_SUCCESS, "Failed to create descriptor pool!");
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		vkDestroyDescriptorPool(m_device.GetDevice(), m_descriptorPool, nullptr);
	}

	bool VulkanDescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptorSet) const
	{
		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = m_descriptorPool;
		allocateInfo.pSetLayouts = &descriptorSetLayout;
		allocateInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case and builds a new pool whenever an old pool fills up.
		VkResult result = vkAllocateDescriptorSets(m_device.GetDevice(), &allocateInfo, &descriptorSet);
		return result == VK_SUCCESS;
	}

	void VulkanDescriptorPool::FreeDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets) const
	{
		vkFreeDescriptorSets(m_device.GetDevice(), m_descriptorPool, static_cast<U32>(descriptorSets.size()), descriptorSets.data());
	}

	void VulkanDescriptorPool::ResetPool()
	{
		vkResetDescriptorPool(m_device.GetDevice(), m_descriptorPool, 0);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Descriptor writer
	/////////////////////////////////////////////////////////////////////////////////

	VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout& descriptorSetLayout, VulkanDescriptorPool& descriptorPool)
		: m_descriptorSetLayout(descriptorSetLayout), m_descriptorPool(descriptorPool)
	{
	}

	VulkanDescriptorWriter& VulkanDescriptorWriter::WriteBuffer(U32 binding, VkDescriptorBufferInfo* bufferInfo)
	{
		ASSERT(m_descriptorSetLayout.m_bindings.count(binding) == 1, "Layout does not contain specified binding!");

		VkDescriptorSetLayoutBinding& bindingDescription = m_descriptorSetLayout.m_bindings[binding];
		ASSERT(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple!");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;
		m_writes.push_back(write);

		return *this;
	}

	VulkanDescriptorWriter& VulkanDescriptorWriter::WriteImage(U32 binding, VkDescriptorImageInfo* imageInfo)
	{
		ASSERT(m_descriptorSetLayout.m_bindings.count(binding) == 1, "Layout does not contain specified binding!");

		VkDescriptorSetLayoutBinding& bindingDescription = m_descriptorSetLayout.m_bindings[binding];
		ASSERT(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple!");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;
		m_writes.push_back(write);

		return *this;
	}

	bool VulkanDescriptorWriter::Build(VkDescriptorSet& descriptorSet)
	{
		bool success = m_descriptorPool.AllocateDescriptorSet(m_descriptorSetLayout.GetDescriptorSetLayout(), descriptorSet);
		if (!success)
		{
			return false;
		}

		Overwrite(descriptorSet);
		return true;
	}

	void VulkanDescriptorWriter::Overwrite(VkDescriptorSet& descriptorSet)
	{
		for (VkWriteDescriptorSet& write : m_writes)
		{
			write.dstSet = descriptorSet;
		}

		vkUpdateDescriptorSets(m_descriptorPool.m_device.GetDevice(), m_writes.size(), m_writes.data(), 0, nullptr);
	}

} // namespace Mapo
