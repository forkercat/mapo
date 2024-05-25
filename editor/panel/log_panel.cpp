//
// Created by Junhao Wang (@forkercat) on 5/25/24.
//

#include "log_panel.h"

#include "engine/ui/imgui_utils.h"

#include <imgui/imgui.h>

namespace Mapo
{
	LogPanel::LogPanel()
		: Panel("Log")
	{
	}

	void LogPanel::OnImGuiRender()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_AlwaysAutoResize;

		ImVec2 displaySize = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos(ImVec2(displaySize.x - ImGuiUI::Padding, displaySize.y - ImGuiUI::Padding), ImGuiCond_None, ImVec2(1.0f, 1.0f));

		ImGui::Begin(GetPanelName().c_str(), nullptr, flags);

		ImGui::PushStyleColor(ImGuiCol_Text, ImGuiTheme::LogText);
		ImGui::Text("%s", Log::GetLastMessage().c_str());
		ImGui::PopStyleColor(1);

		ImGui::End(); // root
	}
} // namespace Mapo
