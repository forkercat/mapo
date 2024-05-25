//
// Created by Junhao Wang (@forkercat) on 5/19/24.
//

#include "info_panel.h"

#include "engine/application.h"
#include "engine/window.h"
#include "engine/renderer/render_context.h"
#include "engine/renderer/renderer.h"

#include "engine/ui/imgui_utils.h"

#include <imgui/imgui.h>

namespace Mapo
{
	InfoPanel::InfoPanel()
		: Panel("Info")
	{
	}

	void InfoPanel::OnImGuiRender()
	{
		Window&	  window = Application::Get().GetWindow();
		Renderer& renderer = RenderContext::GetRenderer();

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_AlwaysAutoResize;

		ImVec2 displaySize = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos(ImVec2(displaySize.x, ImGuiUI::Padding), ImGuiCond_None, ImVec2(1.0f, 0.0f));

		ImGui::Begin(GetPanelName().c_str(), nullptr, flags);

		ImGui::Text("FPS: %.1f  (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

		ImGui::Text("Renderer: Vulkan (%s)", RenderContext::GetRendererAPIVersion().c_str());

		ImGui::Text("Window: %u x %u", window.GetWidth(), window.GetHeight());

		ImGui::Text("Swapchain: %u x %u", renderer.GetSwapchainWidth(), renderer.GetSwapchainHeight());

		ImGui::Text("#Images: %u | #Frames: %u", renderer.GetImageCount(), RenderContext::GetMaxFramesInFlight());

		// ImGui demo
		static bool showDemo = true;
		// ImGui::Checkbox("Show demo", &showDemo);
		if (showDemo)
		{
			ImGui::ShowDemoWindow(&showDemo);
		}

		ImGui::End(); // root
	}
} // namespace Mapo
