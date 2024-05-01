//
// Created by Junhao Wang (@forkercat) on 4/24/24.
//

#include "imgui_system.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

namespace Mapo
{
	static void CheckVulkanResultFn(VkResult result)
	{
		if (result != VK_SUCCESS)
		{
			MP_ERROR("[ImGui Vulkan] Error: VkResult = %d\n", result);
		}
	}

	ImGuiSystem::ImGuiSystem(VulkanWindow& window, VulkanDevice& device, VkRenderPass renderPass, U32 imageCount)
		: m_device(device)
	{
		// Set up descriptor.
		VkDescriptorPoolSize poolSizes[] = {
			// { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			// { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			// { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			// { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			// { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			// { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			// { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			// { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			// { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			// { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
		poolInfo.poolSizeCount = (U32)IM_ARRAYSIZE(poolSizes);
		poolInfo.pPoolSizes = poolSizes;

		VkResult result = vkCreateDescriptorPool(m_device.GetDevice(), &poolInfo, nullptr, &m_descriptorPool);
		ASSERT_EQ(result, VK_SUCCESS, "Failed to create descriptor pool!");

		// Set up ImGui context.
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Set up ImGui DPI.
		const F32 dpi = 2.0f;
		const F32 fontSize = 15.0f;
		io.FontGlobalScale = 1 / dpi;
		io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans/OpenSans-Bold.ttf", fontSize * dpi);
		// io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Regular.ttf", fontSize * dpi);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans/OpenSans-Regular.ttf", fontSize * dpi);

		// Set up style.
		ImGui::StyleColorsDark();

		// Set up GLFW & Vulkan backends.
		ImGui_ImplGlfw_InitForVulkan(window.GetNativeWindow(), true);

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = m_device.GetInstance();
		initInfo.PhysicalDevice = m_device.GetPhysicalDevice();
		initInfo.Device = m_device.GetDevice();
		QueueFamilyIndices familyIndices = m_device.FindPhysicalQueueFamilies();
		ASSERT(familyIndices.IsComplete(), "Failed to initialize ImGui. Queue family indices is not complete!");
		initInfo.QueueFamily = familyIndices.graphicsFamily.value();
		initInfo.Queue = m_device.GetGraphicsQueue();
		initInfo.RenderPass = renderPass;
		initInfo.DescriptorPool = m_descriptorPool;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = imageCount;
		initInfo.CheckVkResultFn = CheckVulkanResultFn;
		// TODO: Pipeline cache is a potential for future optimization, ignoring for now.
		initInfo.PipelineCache = VK_NULL_HANDLE;
		// TODO: Should integrate allocator library such as VMA.
		initInfo.Allocator = VK_NULL_HANDLE;

		ImGui_ImplVulkan_Init(&initInfo);

		// Upload fonts.
		ImGui_ImplVulkan_CreateFontsTexture();
	}

	ImGuiSystem::~ImGuiSystem()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		vkDestroyDescriptorPool(m_device.GetDevice(), m_descriptorPool, nullptr);
	}

	void ImGuiSystem::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame(); // Update input for instance.
		ImGui::NewFrame();
	}

	void ImGuiSystem::Render(VkCommandBuffer commandBuffer)
	{
		ImGui::Render(); // Finalize the draw data.
		ImDrawData* drawData = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
	}

	void ImGuiSystem::Update()
	{
	}

	void ImGuiSystem::RunExample()
	{
		static ImVec4 clearColor{ 1.0f, 1.0f, 0.0f, 1.0f };

		// 1. Show the big demo.
		if (showDemoWindow)
		{
			ImGui::ShowDemoWindow(&showDemoWindow);
		}

		// 2. Show a simple window.
		{
			ImGui::Begin("Hello world!");

			static F32 val = 0.0f;
			static I32 counter = 0;

			ImGui::Text("This is some useful text.");
			ImGui::Checkbox("Demo Window", &showDemoWindow);
			ImGui::Checkbox("Another Window", &showAnotherWindow);

			ImGui::SliderFloat("Float", &val, 0.0f, 1.0f);
			ImGui::ColorEdit3("Clear color", (F32*)&clearColor);

			if (ImGui::Button("Button"))
				++counter;

			ImGui::SameLine();
			ImGui::Text("Counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::End();
		}

		// 3. Show another simple window.
		if (showAnotherWindow)
		{
			ImGui::Begin("Another Window", &showDemoWindow);
			ImGui::Text("Hello from  another window!");

			if (ImGui::Button("Close Me!"))
			{
				showAnotherWindow = false;
			}

			ImGui::End();
		}
	}

} // namespace Mapo
