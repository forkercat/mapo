//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#include "device.h"

#include "engine/window.h"
#include "engine/renderer/vk_common.h"

#include <unordered_set>
#include <set>

namespace Mapo
{
	/////////////////////////////////////////////////////////////////////////////////
	// Local callback functions
	/////////////////////////////////////////////////////////////////////////////////

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT														   messageType,
		const VkDebugUtilsMessengerCallbackDataEXT*											   pCallbackData,
		void*																				   pUserData)
	{
		MP_DEBUG("Validation Output: {}", pCallbackData->pMessage);
		return VK_FALSE; // Original Vulkan call is not aborted
	}

	// Proxy functions to create and destroy debug messenger.
	// Since this function is an extension function, it is not automatically loaded.
	// We have to look up its address ourselves.
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT*			pCreateInfo,
		const VkAllocationCallbacks*						pAllocator,
		VkDebugUtilsMessengerEXT*							pDebugMessenger)
	{
		auto func =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func =
			(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		MP_ASSERT(func, "Function to destroy debug messenger could not be found!");

		if (func)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Class member functions
	/////////////////////////////////////////////////////////////////////////////////

	Device::Device(Window& window)
		: m_window(window)
	{
		CreateInstance();
		SetUpDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateCommandPool();
	}

	Device::~Device()
	{
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
		vkDestroyDevice(m_device, nullptr);

		if (m_enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyInstance(m_instance, nullptr);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Public functions
	/////////////////////////////////////////////////////////////////////////////////

	void Device::WaitIdle()
	{
		VK_CHECK(vkDeviceWaitIdle(m_device));
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Public helper functions
	/////////////////////////////////////////////////////////////////////////////////

	U32 Device::FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags propertyFlags)
	{
		// If typeFilter is 0000 1100, the function will return an index of 2.
		// Memory heaps are distinct memory resources like dedicated VRAM and swap space in RAM for when VRAM runs out.
		// The different types of memory exist within these heaps.

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(m_gpu, &memoryProperties);

		// MP_PRINT("Memory type count: %u | heap count: %u", memoryProperties.memoryTypeCount,
		// memoryProperties.memoryHeapCount);

		for (U32 typeIndex = 0; typeIndex < memoryProperties.memoryTypeCount; typeIndex++)
		{
			if (typeFilter & (1 << typeIndex))
			{
				VkMemoryType memoryType = memoryProperties.memoryTypes[typeIndex];

				// Check if the desired property flags are all matched.
				if ((memoryType.propertyFlags & propertyFlags) == propertyFlags)
				{
					return typeIndex;
				}
			}
		}

		MP_ASSERT(false, "Failed to find suitable memory type!");
		return -1;
	}

	VkFormat Device::FindSupportedFormat(const std::vector<VkFormat>& formatCandidates, VkImageTiling tiling,
		VkFormatFeatureFlags features)
	{
		for (const VkFormat& format : formatCandidates)
		{
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(m_gpu, format, &properties);

			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		MP_ASSERT(false, "Failed to find supported format!");
		return {};
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Buffer helper functions
	/////////////////////////////////////////////////////////////////////////////////

	void Device::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		// Buffer creation
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usageFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // only used by the graphics queue

		VK_CHECK(vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer));

		// Memory allocation
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(m_device, buffer, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		// To be able to write to it from CPU.
		allocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);

		VK_CHECK(vkAllocateMemory(m_device, &allocateInfo, nullptr, &bufferMemory));

		// Bind buffer and allocation.
		vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
	}

	void Device::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(commandBuffer);
	}

	void Device::CopyBufferToImage(VkBuffer buffer, VkImage image, U32 width, U32 height, U32 layerCount)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy copyRegion{};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = layerCount;

		copyRegion.imageOffset = { 0, 0, 0 };
		copyRegion.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		EndSingleTimeCommands(commandBuffer);
	}

	VkCommandBuffer Device::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_commandPool;
		allocateInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device, &allocateInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void Device::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_graphicsQueue);

		vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Image helper functions
	/////////////////////////////////////////////////////////////////////////////////

	void Device::CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags propertyFlags,
		VkImage& image, VkDeviceMemory& imageMemory)
	{
		VK_CHECK(vkCreateImage(m_device, &imageInfo, nullptr, &image));

		// Allocate memory.
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(m_device, image, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);

		VK_CHECK(vkAllocateMemory(m_device, &allocateInfo, nullptr, &imageMemory));
		VK_CHECK(vkBindImageMemory(m_device, image, imageMemory, 0));
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Functions to create Vulkan resources
	/////////////////////////////////////////////////////////////////////////////////

	void Device::CreateInstance()
	{
		if (m_enableValidationLayers && !CheckValidationLayerSupport())
		{
			MP_ASSERT(false, "Validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "LittleVulkanEngine App";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Example: VK_KHR_surface, VK_EXT_metal_surface, VK_KHR_portability_enumeration
		std::vector<const char*> requiredExtensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<U32>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		// Additional settings for macOS, otherwise you would get VK_ERROR_INCOMPATIBLE_DRIVER.
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

		// Placed outside if for longer lifecycle before instance will be created.
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		if (m_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<U32>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();

			// Needed for debugging issues in the vkCreateInstance and vkDestroyInstance calls.
			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance));

		HasGlfwRequiredInstanceExtensions();
	}

	void Device::SetUpDebugMessenger()
	{
		if (m_enableValidationLayers)
		{
			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			PopulateDebugMessengerCreateInfo(createInfo);

			VK_CHECK(CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger));
		}
	}

	void Device::CreateSurface()
	{
		void* pInstance = (void*)&m_instance;
		void* pSurface = (void*)&m_surface;
		m_window.CreateWindowSurface(pInstance, pSurface);
	}

	void Device::PickPhysicalDevice()
	{
		U32 deviceCount{};
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

		MP_ASSERT_NEQ(deviceCount, 0, "Failed to find GPUs with Vulkan support!");

		MP_INFO("Device count: {}", deviceCount);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				m_gpu = device;
				break;
			}
		}

		MP_ASSERT_NEQ(m_gpu, VK_NULL_HANDLE, "Failed to find a suitable GPU device!");

		vkGetPhysicalDeviceProperties(m_gpu, &properties);
		MP_INFO("Physical device: {}", properties.deviceName);
	}

	void Device::CreateLogicalDevice()
	{
		// Queue families
		QueueFamilyIndices queueFamilyData = FindQueueFamilies(m_gpu);

		// If the queue families are the same, then we only need to pass its index once.
		std::set<U32> uniqueQueueFamilies = { queueFamilyData.graphicsFamily.value(),
			queueFamilyData.presentFamily.value() };

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		F32									 queuePriority = 1.0f;

		for (U32 queueFamilyIndex : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Device features
		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<U32>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<U32>(m_deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_deviceExtensions.data(); // e.g. swap chain

		// Might not really be necessary anymore because device specific validation layers
		// have been deprecated.
		if (m_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<U32>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		VK_CHECK(vkCreateDevice(m_gpu, &createInfo, nullptr, &m_device));

		// Fetch queue handle.
		vkGetDeviceQueue(m_device, queueFamilyData.graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, queueFamilyData.presentFamily.value(), 0, &m_presentQueue);
	}

	void Device::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_gpu);

		// We will be recording a command buffer every frame, so we want to be able to reset and
		// record over it again.
		VkCommandPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		// Command buffers are executed by submitting them on one of the device queues, e.g. graphics queue.
		VK_CHECK(vkCreateCommandPool(m_device, &poolCreateInfo, nullptr, &m_commandPool));
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Private helper functions
	/////////////////////////////////////////////////////////////////////////////////

	bool Device::IsDeviceSuitable(VkPhysicalDevice physicalDevice)
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);
		bool			   extensionsSupported = CheckDeviceExtensionSupport(physicalDevice);

		// It is important that we only query for swap chain support after verifying that the extensions are available.
		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapchainSupportDetails swapChainSupport = QuerySwapchainSupport(physicalDevice);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedDeviceFeatures{};
		vkGetPhysicalDeviceFeatures(physicalDevice, &supportedDeviceFeatures);

		return queueFamilyIndices.IsComplete() && extensionsSupported && swapChainAdequate && supportedDeviceFeatures.samplerAnisotropy;
	}

	std::vector<const char*> Device::GetRequiredExtensions()
	{
		U32			 glfwExtensionCount = 0;
		const char** glfwExtensions = m_window.GlfwGetRequiredExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_enableValidationLayers)
		{
			extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		// Additional settings for macOS, otherwise you would get VK_ERROR_INCOMPATIBLE_DRIVER.
		extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

		// Fixing the device error on macOS.
		extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		return extensions;
	}

	bool Device::CheckValidationLayerSupport()
	{
		U32 layerCount{};
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Make sure all the layer names are present.
		for (const char* layerName : m_validationLayers)
		{
			auto it = std::find_if(availableLayers.begin(), availableLayers.end(),
				[layerName](VkLayerProperties layerProperties) {
					return strcmp(layerProperties.layerName, layerName);
				});

			if (it == availableLayers.end())
			{
				return false;
			}
		}

		return true;
	}

	QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice physicalDevice)
	{
		U32 queueFamilyCount{};
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		// Find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT.
		QueueFamilyIndices queueFamilyData;
		U32				   queueFamilyIndex = 0;

		for (const auto& queueFamily : queueFamilies)
		{
			// Graphics
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queueFamilyData.graphicsFamily = queueFamilyIndex;
			}

			// Present
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, m_surface, &presentSupport);

			if (presentSupport)
			{
				queueFamilyData.presentFamily = queueFamilyIndex;
			}

			if (queueFamilyData.IsComplete())
			{
				break;
			}

			queueFamilyIndex++;
		}

		return queueFamilyData;
	}

	void Device::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		createInfo.pfnUserCallback = DebugCallback;
		createInfo.pUserData = nullptr;
	}

	void Device::HasGlfwRequiredInstanceExtensions()
	{
		U32 extensionCount{};
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		printf("Available extensions:\n");
		std::unordered_set<std::string> availableExtensionSet;

		for (const auto& extension : extensions)
		{
			printf("\t%s", extension.extensionName);
			availableExtensionSet.insert(extension.extensionName);
		}

		MP_NEWLINE();

		printf("Required extensions:\n");
		const std::vector<const char*> requiredExtensions = GetRequiredExtensions();

		for (const auto& required : requiredExtensions)
		{
			printf("\t%s", required);

			if (availableExtensionSet.find(required) == availableExtensionSet.end())
			{
				MP_ERROR("Missing required GLFW extension: %s", required);
			}
		}

		MP_NEWLINE();
	}

	bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
	{
		U32 extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapchainSupportDetails Device::QuerySwapchainSupport(VkPhysicalDevice physicalDevice)
	{
		SwapchainSupportDetails details{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &details.capabilities);

		U32 formatCount{};
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, details.formats.data());
		}

		U32 presentModeCount{};
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount,
				details.presentModes.data());
		}

		return details;
	}

} // namespace Mapo
