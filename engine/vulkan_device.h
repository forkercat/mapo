//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#pragma once

#include "engine/window.h"

#include <vector>
#include <optional>

namespace mapo
{
	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices
	{
		std::optional<U32> graphicsFamily;
		std::optional<U32> presentFamily;

		bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	// Device class that manages Vulkan resources such as Vulkan instance, physical device, logical device,
	// pool, surface, and queues. It also provides useful helper functions for buffer and image creation.
	class VulkanDevice
	{
	public:
		VulkanDevice(Window& window);
		~VulkanDevice();

		// Make the device not copiable or movable.
		VulkanDevice(const VulkanDevice&) = delete;
		void operator=(const VulkanDevice&) = delete;
		VulkanDevice(VulkanDevice&&) = delete;
		VulkanDevice& operator=(VulkanDevice&&) = delete;

		// Getter for Vulkan resources
		VkCommandPool GetCommandPool() { return m_commandPool; }
		VkDevice GetDevice() { return m_device; }
		VkSurfaceKHR GetSurface() { return m_surface; }
		VkQueue GetGraphicsQueue() { return m_graphicsQueue; }
		VkQueue GetPresentQueue() { return m_presentQueue; }

		// Public helper functions
		SwapchainSupportDetails GetSwapchainSupport() { return QuerySwapchainSupport(m_physicalDevice); };
		QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_physicalDevice); }

		U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags propertyFlags);
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& formatCandidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer helper functions
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, U32 width, U32 height, U32 layerCount);

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		// Image helper functions
		void CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags propertyFlags, VkImage& image,
			VkDeviceMemory& imageMemory);

	private:
		// Functions to create Vulkan resources
		void CreateInstance();
		void SetUpDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		// Private help functions
		bool IsDeviceSuitable(VkPhysicalDevice physicalDevice);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void HasGlfwRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
		SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice);

	public:
		VkPhysicalDeviceProperties properties;

	private:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		Window& m_window;
		VkCommandPool m_commandPool;

		VkDevice m_device;
		VkSurfaceKHR m_surface;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

#ifdef NDBUG
		const bool m_enableValidationLayers = false;
#else
		const bool m_enableValidationLayers = true;
#endif

		const std::vector<const char*> m_validationLayers{ "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };
	};

} // namespace mapo
