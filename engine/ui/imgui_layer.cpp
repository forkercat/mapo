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

#include "engine/ui/imgui_utils.h"

#include <vulkan/vulkan.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

#include <ImGuizmo.h>
#include <IconFontCppHeaders/IconsFontAwesome5.h>

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
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Set up ImGui DPI.
		const F32 fontSize = 14.0f;
		const F32 iconSize = 10.0f;
		io.FontGlobalScale = 1 / ImGuiConfig::Dpi;
		// io.Fonts->AddFontFromFileTTF("assets/fonts/Ruda/Ruda-Bold.ttf", fontSize * dpi);
		io.Fonts->AddFontFromFileTTF("assets/fonts/Ruda/Ruda-Regular.ttf", fontSize * ImGuiConfig::Dpi);
		// io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans/OpenSans-Bold.ttf", fontSize * dpi);
		// io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Regular.ttf", fontSize * dpi);
		// io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans/OpenSans-Regular.ttf", fontSize * dpi);

		// Icons
		// static const ImWchar iconRanges[] = { 0xe00f, 0xf8ff, 0 };
		static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		static ImFontConfig	 config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = iconSize;
		io.Fonts->AddFontFromFileTTF("assets/fonts/FontAwesome5/fa-solid-900.ttf", iconSize * ImGuiConfig::Dpi, &config, iconRanges);

		// Set up style.
		ImGui::StyleColorsDark();
		SetStyles();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
		// ones.
		// ImGuiStyle& style = ImGui::GetStyle();
		// if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		// {
		// 	style.WindowRounding = 0.f;
		// 	style.Colors[ImGuiCol_WindowBg].w = 1.f;
		// }

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

		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Window&	 window = Application::Get().GetWindow();
		io.DisplaySize = ImVec2(window.GetWidth(), window.GetHeight());

		ImGui::Render();

		VkCommandBuffer currentCmdBuffer = RenderContext::GetRenderer().GetCurrentCommandBuffer();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), currentCmdBuffer);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			void* backupCurrentContext = window.GetCurrentContext();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			window.MakeCurrentContext(backupCurrentContext);
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
		// RunExample();
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
		// TODO: Add comment.
		if (m_blockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.handled |= event.InCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.handled |= event.InCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::SetStyles()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		// style.WindowMinSize.x = 200.0f;

		// Spacing
		style.ItemSpacing.y = 5.0f;

		// Rounding
		style.WindowRounding = 4.0f;
		style.ChildRounding = 4.0f;
		style.FrameRounding = 4.0f;
		style.TabRounding = 4.0f;
		style.PopupRounding = 4.0f;
		style.GrabRounding = 3.0f;

		// Padding
		style.FramePadding = ImVec2(6, 3);

		// Size
		style.GrabMinSize = 11.0f;

		// SeparatorText
		style.SeparatorTextAlign.x = 0.48f;

		// Show/Hide window menu button
		style.WindowMenuButtonPosition = ImGuiDir_None;

		// Colors
		static bool useCustomColor = true;
		if (useCustomColor)
		{
			auto& colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_WindowBg] = ImColor(ImGuiTheme::WindowBg);
			colors[ImGuiCol_PopupBg] = ImColor(ImGuiTheme::PopupBg);
			colors[ImGuiCol_Text] = ImColor(ImGuiTheme::Text);

			// Use the main theme color
			colors[ImGuiCol_DockingPreview] = ImColor(ImGuiTheme::MainTheme);
			colors[ImGuiCol_CheckMark] = ImColor(ImGuiTheme::MainTheme);
			colors[ImGuiCol_SliderGrab] = ImColor(ImGuiTheme::MainTheme);
			colors[ImGuiCol_SliderGrabActive] = ImColor(ImGuiTheme::MainTheme);
			colors[ImGuiCol_ResizeGrip] = ImColor(ImGuiTheme::MainTheme);

			// Headers
			colors[ImGuiCol_Header] = ImColor(ImGuiTheme::Normal);
			colors[ImGuiCol_HeaderHovered] = ImColor(ImGuiTheme::Normal);
			colors[ImGuiCol_HeaderActive] = ImColor(ImGuiTheme::Normal); // change to normal

			// Buttons
			colors[ImGuiCol_Button] = ImColor(ImGuiTheme::Normal);
			colors[ImGuiCol_ButtonHovered] = ImColor(ImGuiTheme::Hovered);
			colors[ImGuiCol_ButtonActive] = ImColor(ImGuiTheme::Active);

			// Frame BG
			colors[ImGuiCol_FrameBg] = ImColor(ImGuiTheme::Normal);
			colors[ImGuiCol_FrameBgHovered] = ImColor(ImGuiTheme::Hovered);
			colors[ImGuiCol_FrameBgActive] = ImColor(ImGuiTheme::Active);

			// Tabs
			colors[ImGuiCol_Tab] = ImColor(ImGuiTheme::TabNormal);
			colors[ImGuiCol_TabHovered] = ImColor(ImGuiTheme::TabHovered);
			colors[ImGuiCol_TabActive] = ImColor(ImGuiTheme::TabActive);
			colors[ImGuiCol_TabUnfocused] = ImColor(ImGuiTheme::TabUnfocused);
			colors[ImGuiCol_TabUnfocusedActive] = ImColor(ImGuiTheme::TabUnfocusedActive);

			// Title
			colors[ImGuiCol_TitleBg] = ImColor(ImGuiTheme::TitleBg);
			colors[ImGuiCol_TitleBgActive] = ImColor(ImGuiTheme::TitleBgActive);
			colors[ImGuiCol_TitleBgCollapsed] = ImColor(ImGuiTheme::TitleBgCollapsed);

			// Separator
			colors[ImGuiCol_Separator] = ImColor(ImGuiTheme::Separator);
			colors[ImGuiCol_SeparatorHovered] = ImColor(ImGuiTheme::SeparatorHovered);
			colors[ImGuiCol_SeparatorActive] = ImColor(ImGuiTheme::SeparatorActive);

			// ScrollBar
			colors[ImGuiCol_TitleBg] = ImColor(ImGuiTheme::TitleBg);
		}
	}

} // namespace Mapo
