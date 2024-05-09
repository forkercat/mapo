//
// Created by Junhao Wang (@forkercat) on 4/21/24.
//

#include "vulkan_buffer.h"

namespace Mapo
{
	VulkanBuffer::VulkanBuffer(
		VulkanDevice& device,
		VkDeviceSize instanceSize,
		U32 instanceCount,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags,
		VkDeviceSize minOffsetAlignment)
		: m_device(device),
		  m_instanceSize(instanceSize),
		  m_instanceCount(instanceCount),
		  m_usageFlags(usageFlags),
		  m_memoryPropertyFlags(memoryPropertyFlags)
	{
		m_alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
		m_bufferSize = m_alignmentSize * instanceCount;
		m_device.CreateBuffer(m_bufferSize, m_usageFlags, m_memoryPropertyFlags, m_buffer, m_memory);

		MP_PRINT("New buffer of (%llu x %u) bytes (actual: %llu bytes), with min offset alignment: %llu",
			instanceSize, instanceCount, m_bufferSize, minOffsetAlignment);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		Unmap();
		vkDestroyBuffer(m_device.GetDevice(), m_buffer, nullptr);
		vkFreeMemory(m_device.GetDevice(), m_memory, nullptr);
	}

	VkResult VulkanBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
	{
		MP_ASSERT(m_buffer && m_memory, "Failed to map buffer since buffer resources are not created!");

		// Map the host memory on CPU to the device memory on GPU.
		// memcpy will copy the vertices data memory to the memory on CPU.
		// Since we use COHERENT flag, the CPU memory will automatically be flushed to update GPU memory.
		return vkMapMemory(m_device.GetDevice(), m_memory, offset, size, 0, &m_mappedData);
	}

	void VulkanBuffer::Unmap()
	{
		if (m_mappedData)
		{
			vkUnmapMemory(m_device.GetDevice(), m_memory);
			m_mappedData = nullptr;
		}
	}

	void VulkanBuffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		MP_ASSERT(m_mappedData, "Could not copy to unmapped buffer!");
		if (size == VK_WHOLE_SIZE)
		{
			memcpy(m_mappedData, data, m_bufferSize);
		}
		else
		{
			char* memoryOffset = (char*)m_mappedData;
			memoryOffset += offset;
			memcpy(memoryOffset, data, size);
		}
	}

	VkResult VulkanBuffer::Flush(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange{};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(m_device.GetDevice(), 1, &mappedRange);
	}

	VkDescriptorBufferInfo VulkanBuffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
	{
		return { m_buffer, offset, size };
	}

	VkResult VulkanBuffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange{};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(m_device.GetDevice(), 1, &mappedRange);
	}

	void VulkanBuffer::WriteToIndex(void* data, U32 index)
	{
		WriteToBuffer(data, m_instanceSize, index * m_alignmentSize);
	}

	VkResult VulkanBuffer::FlushAtIndex(U32 index)
	{
		return Flush(m_alignmentSize, index * m_alignmentSize);
	}

	VkResult VulkanBuffer::InvalidateAtIndex(U32 index)
	{
		return Invalidate(m_alignmentSize, index * m_alignmentSize);
	}

	VkDescriptorBufferInfo VulkanBuffer::DescriptorInfoAtIndex(U32 index)
	{
		return DescriptorInfo(m_alignmentSize, index * m_alignmentSize);
	}

	VkDeviceSize VulkanBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
	{
		if (minOffsetAlignment > 0)
		{
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		else
		{
			return instanceSize;
		}
	}

} // namespace Mapo
