//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#include "imgui_layer.h"

#include "engine/application.h"
#include "engine/window.h"

#include "engine/renderer/vk_common.h"
#include "engine/renderer/render_context.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/device.h"

#include <vulkan/vulkan.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

namespace Mapo
{
	// We haven't yet covered descriptor pools in the tutorial series,
	// so I'm just going to create one for just imgui and store it here for now.
	// maybe its preferred to have a separate descriptor pool for imgui anyway.
	static VkDescriptorPool s_descriptorPool = VK_NULL_HANDLE;

	static void CheckVulkanResultFn(VkResult result)
	{
		if (result != VK_SUCCESS)
		{
			MP_ERROR("[ImGui] Error: VkResult = {}\n", (int)result);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer"), m_device(RenderContext::GetDevice())
	{
	}

	void ImGuiLayer::OnAttach()
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

		VK_CHECK(vkCreateDescriptorPool(m_device.GetDevice(), &poolInfo, nullptr, &s_descriptorPool));

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
		Window& window = Application::Get().GetWindow();
		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(window.GetNativeWindow()), true);

		Renderer& renderer = RenderContext::GetRenderer();

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = m_device.GetInstance();
		initInfo.PhysicalDevice = m_device.GetPhysicalDevice();
		initInfo.Device = m_device.GetDevice();
		QueueFamilyIndices familyIndices = m_device.FindPhysicalQueueFamilies();
		MP_ASSERT(familyIndices.IsComplete(), "Failed to initialize ImGui. Queue family indices is not complete!");
		initInfo.QueueFamily = familyIndices.graphicsFamily.value();
		initInfo.Queue = m_device.GetGraphicsQueue();
		initInfo.RenderPass = renderer.GetRenderPass();
		initInfo.DescriptorPool = s_descriptorPool;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = renderer.GetImageCount();
		initInfo.CheckVkResultFn = CheckVulkanResultFn;
		// TODO: Pipeline cache is a potential for future optimization, ignoring for now.
		initInfo.PipelineCache = VK_NULL_HANDLE;
		// TODO: Should integrate allocator library such as VMA.
		initInfo.Allocator = VK_NULL_HANDLE;

		ImGui_ImplVulkan_Init(&initInfo);

		// Upload fonts.
		ImGui_ImplVulkan_CreateFontsTexture();
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		vkDestroyDescriptorPool(m_device.GetDevice(), s_descriptorPool, nullptr);
		s_descriptorPool = VK_NULL_HANDLE;
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame(); // Update input for instance.
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		// ?
		// ImGuiIO& io = ImGui::GetIO();
		// Application& app = Application::Get();
		// io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui::Render();

		VkCommandBuffer currentCmdBuffer = RenderContext::GetRenderer().GetCurrentCommandBuffer();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), currentCmdBuffer);

		// ?
		// if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		// {
		// 	GLFWwindow* backup_current_context = glfwGetCurrentContext();
		// 	ImGui::UpdatePlatformWindows();
		// 	ImGui::RenderPlatformWindowsDefault();
		// 	glfwMakeContextCurrent(backup_current_context);
		// }
	}

	void ImGuiLayer::OnImGuiRender()
	{
		RunExample();
	}

	void ImGuiLayer::RunExample()
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

	void ImGuiLayer::OnEvent(Event& event)
	{
		if (m_blockEvents)
		{
		}
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		/*
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		*/
	}

} // namespace Mapo
