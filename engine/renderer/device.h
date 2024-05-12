//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#pragma once

#include "core/core.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mapo
{
	class Window;

	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR		capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>	presentModes;
	};

	struct QueueFamilyIndices
	{
		Optional<U32> graphicsFamily;
		Optional<U32> presentFamily;

		bool IsComplete() { return graphicsFamily.HasValue() && presentFamily.HasValue(); }
	};

	// Device class that manages Vulkan resources such as Vulkan instance, physical device, logical device,
	// pool, surface, and queues. It also provides useful helper functions for buffer and image creation.
	class Device final
	{
	public:
		Device(Window& window);
		~Device();

		// Make the device not copiable or movable.
		Device(const Device&) = delete;
		Device operator=(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(Device&&) = delete;

		// Public functions
		void WaitIdle();

		// Getter for Vulkan resources
		VkCommandPool	 GetCommandPool() { return m_commandPool; }
		VkDevice		 GetDevice() { return m_device; }
		VkPhysicalDevice GetPhysicalDevice() { return m_gpu; }
		VkInstance		 GetInstance() { return m_instance; }
		VkSurfaceKHR	 GetSurface() { return m_surface; }
		VkQueue			 GetGraphicsQueue() { return m_graphicsQueue; }
		VkQueue			 GetPresentQueue() { return m_presentQueue; }

		// Public helper functions
		SwapchainSupportDetails GetSwapchainSupport() { return QuerySwapchainSupport(m_gpu); };
		QueueFamilyIndices		FindPhysicalQueueFamilies() { return FindQueueFamilies(m_gpu); }

		U32		 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags propertyFlags);
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& formatCandidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer helper functions
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, U32 width, U32 height, U32 layerCount);

		VkCommandBuffer BeginSingleTimeCommands();
		void			EndSingleTimeCommands(VkCommandBuffer commandBuffer);

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
		bool					 IsDeviceSuitable(VkPhysicalDevice physicalDevice);
		std::vector<const char*> GetRequiredExtensions();
		bool					 CheckValidationLayerSupport();
		QueueFamilyIndices		 FindQueueFamilies(VkPhysicalDevice physicalDevice);
		void					 PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void					 HasGlfwRequiredInstanceExtensions();
		bool					 CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
		SwapchainSupportDetails	 QuerySwapchainSupport(VkPhysicalDevice physicalDevice);

	public:
		VkPhysicalDeviceProperties properties;

	private:
		Window& m_window;

		VkInstance				 m_instance = VK_NULL_HANDLE;
		VkDevice				 m_device = VK_NULL_HANDLE;
		VkPhysicalDevice		 m_gpu = VK_NULL_HANDLE;
		VkCommandPool			 m_commandPool = VK_NULL_HANDLE;
		VkSurfaceKHR			 m_surface = VK_NULL_HANDLE;
		VkQueue					 m_graphicsQueue = VK_NULL_HANDLE;
		VkQueue					 m_presentQueue = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

#ifdef NDBUG
		const bool m_enableValidationLayers = false;
#else
		const bool m_enableValidationLayers = true;
#endif

		const std::vector<const char*> m_validationLayers{ "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };
	};

} // namespace Mapo
