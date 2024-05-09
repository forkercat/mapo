//
// Created by Junhao Wang (@forkercat) on 4/21/24.
//

#pragma once

#include "platform/vulkan/vulkan_device.h"

namespace Mapo
{
	class Buffer
	{
	public:
		constexpr static U64 WHOLE_SIZE = (~0ULL);

		enum class Usage
		{
			NONE = 0,
			UNIFORM_BIT = BIT(0),
		};

		enum class Property
		{
			NONE = 0,
			HOST_VISIBLE = BIT(0),
		};

		Buffer(U64 instanceSize, U32 instanceCount, Usage usage, Property property, U64 minOffsetAlignment = 1)
		{
		}

		virtual ~Buffer() = default;

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		// Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
		virtual bool Map(U64 size = WHOLE_SIZE, U64 offset = 0) = 0;

		// Unmap a mapped memory range.
		virtual void Unmap() = 0;


	};

	// Encapsulates a vulkan buffer. Initially based off VulkanBuffer by Sascha Willems.
	// https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
	class VulkanBuffer
	{
	public:
		VulkanBuffer(
			VulkanDevice& device,
			VkDeviceSize instanceSize,
			U32 instanceCount,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags,
			VkDeviceSize minOffsetAlignment = 1);
		~VulkanBuffer();

		VulkanBuffer(const VulkanBuffer&) = delete;
		VulkanBuffer& operator=(const VulkanBuffer&) = delete;

		// Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		// Unmap a mapped memory range.
		void Unmap();

		// Copy the specified data to the mapped buffer. Default value writes the whole buffer range.
		void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		// Flush a memory range of the buffer to make it visible to the GPU device. Only required for non-coherent memory.
		VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		// Invalidate a memory range of the buffer to make it visible to the host.
		VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		// Create a buffer info descriptor.
		VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		// Copy "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize.
		void WriteToIndex(void* data, U32 index);
		// Flush the memory range at index * alignmentSize of the buffer to make it visible to the GPU device.
		VkResult FlushAtIndex(U32 index);
		VkResult InvalidateAtIndex(U32 index);
		VkDescriptorBufferInfo DescriptorInfoAtIndex(U32 index);

		// Getters
		VkBuffer GetBuffer() const { return m_buffer; }
		void* GetMappedMemory() const { return m_mappedData; }
		U32 GetInstanceCount() const { return m_instanceCount; }
		VkDeviceSize GetInstanceSize() const { return m_instanceSize; }
		VkDeviceSize GetAlignmentSize() const { return m_alignmentSize; }
		VkBufferUsageFlags GetUsageFlags() const { return m_usageFlags; }
		VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return m_memoryPropertyFlags; }
		VkDeviceSize GetBufferSize() const { return m_bufferSize; }

	private:
		// Returns the minimum instance size required to be compatible with the device's minimum offset alignment.
		// - GetAlignment(9, 0) returns 9
		// - GetAlignment(9, 1) returns 1
		// - GetAlignment(9, 8) returns 16
		// - GetAlignment(9, 16) returns 16
		static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

	private:
		VulkanDevice& m_device;
		void* m_mappedData = nullptr;
		VkBuffer m_buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_memory = VK_NULL_HANDLE;

		VkDeviceSize m_bufferSize;
		U32 m_instanceCount;
		VkDeviceSize m_instanceSize;
		VkDeviceSize m_alignmentSize;
		VkBufferUsageFlags m_usageFlags;
		VkMemoryPropertyFlags m_memoryPropertyFlags;
	};

} // namespace Mapo
