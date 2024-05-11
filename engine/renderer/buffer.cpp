//
// Created by Junhao Wang (@forkercat) on 4/21/24.
//

#include "buffer.h"

namespace Mapo
{
	Buffer::Buffer(
		Device& device,
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

		MP_DEBUG("New buffer of ({} x {}) bytes (actual: {} bytes), with min offset alignment: {}",
			instanceSize, instanceCount, m_bufferSize, minOffsetAlignment);
	}

	Buffer::~Buffer()
	{
		Unmap();
		vkDestroyBuffer(m_device.GetDevice(), m_buffer, nullptr);
		vkFreeMemory(m_device.GetDevice(), m_memory, nullptr);
	}

	VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset)
	{
		MP_ASSERT(m_buffer && m_memory, "Failed to map buffer since buffer resources are not created!");

		// Map the host memory on CPU to the device memory on GPU.
		// memcpy will copy the vertices data memory to the memory on CPU.
		// Since we use COHERENT flag, the CPU memory will automatically be flushed to update GPU memory.
		return vkMapMemory(m_device.GetDevice(), m_memory, offset, size, 0, &m_mappedData);
	}

	void Buffer::Unmap()
	{
		if (m_mappedData)
		{
			vkUnmapMemory(m_device.GetDevice(), m_memory);
			m_mappedData = nullptr;
		}
	}

	void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
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

	VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange{};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(m_device.GetDevice(), 1, &mappedRange);
	}

	VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
	{
		return { m_buffer, offset, size };
	}

	VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange{};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(m_device.GetDevice(), 1, &mappedRange);
	}

	void Buffer::WriteToIndex(void* data, U32 index)
	{
		WriteToBuffer(data, m_instanceSize, index * m_alignmentSize);
	}

	VkResult Buffer::FlushAtIndex(U32 index)
	{
		return Flush(m_alignmentSize, index * m_alignmentSize);
	}

	VkResult Buffer::InvalidateAtIndex(U32 index)
	{
		return Invalidate(m_alignmentSize, index * m_alignmentSize);
	}

	VkDescriptorBufferInfo Buffer::DescriptorInfoAtIndex(U32 index)
	{
		return DescriptorInfo(m_alignmentSize, index * m_alignmentSize);
	}

	VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
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
