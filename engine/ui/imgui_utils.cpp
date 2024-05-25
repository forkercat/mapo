//
// Created by Junhao Wang (@forkercat) on 5/23/24.
//

#include "imgui_utils.h"

#include <ImGuizmo.h>

#include <IconFontCppHeaders/IconsFontAwesome5.h>

namespace Mapo
{
	void ImGuiUtils::DrawControlFloat3(const char* label, Vector3& values, const char* format, F32 resetValue, F32 columnWidth)
	{
		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label);
		ImGui::NextColumn();

		// ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushItemWidth(ImGui::CalcItemWidth());
		ImGui::PushItemWidth(ImGui::CalcItemWidth());
		ImGui::PushItemWidth(ImGui::CalcItemWidth());

		const F32 itemInnerSpacing = 4;
		const F32 itemOutterSpacing = 4;

		// X
		ImGui::PushStyleColor(ImGuiCol_Button, ImGuiTheme::ComponentX);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiTheme::ComponentHoveredX);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiTheme::ComponentX);
		if (ImGui::Button("X", ImVec2(0, 0)))
		{
			values.x = resetValue;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine(0, itemInnerSpacing);
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, format);
		ImGui::PopItemWidth();
		ImGui::SameLine(0, itemOutterSpacing);

		// Y
		ImGui::PushStyleColor(ImGuiCol_Button, ImGuiTheme::ComponentY);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiTheme::ComponentHoveredY);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiTheme::ComponentY);
		if (ImGui::Button("Y", ImVec2(0, 0)))
		{
			values.y = resetValue;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine(0, itemInnerSpacing);
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, format);
		ImGui::PopItemWidth();
		ImGui::SameLine(0, itemOutterSpacing);

		// Z
		ImGui::PushStyleColor(ImGuiCol_Button, ImGuiTheme::ComponentZ);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiTheme::ComponentHoveredZ);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiTheme::ComponentZ);
		if (ImGui::Button("Z", ImVec2(0, 0)))
		{
			values.z = resetValue;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine(0, itemInnerSpacing);
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, format);
		ImGui::PopItemWidth();

		ImGui::Columns(1, nullptr, false);
		ImGui::PopID(); // pop ID label
	}

	void ImGuiUtils::DrawGizmoControls(int& currentGizmoType)
	{
		static U32	  numButton = 4;
		static ImVec2 buttonSize{ 32.0f, 23.0f };

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(ImGuiUI::Padding, ImGuiUI::Padding));

		ImVec2 displaySize = ImGui::GetIO().DisplaySize;

		ImVec2 controlSize = ImVec2(buttonSize.x * numButton + ImGuiUI::Padding * (numButton - 1) + ImGuiUI::Padding * 2, buttonSize.y + ImGuiUI::Padding * 2);

		ImGui::SetNextWindowPos(ImVec2(displaySize.x / 2.0f, ImGuiUI::Padding * 2.0f), ImGuiCond_None, ImVec2(0.5f, 0.0f));
		ImGui::SetNextWindowSize(controlSize, ImGuiCond_None);

		ImGui::Begin("GizmoControls", nullptr, flags);

		if (ImGuiUtils::DrawGizmoControlButton(ICON_FA_HAND_PAPER, INVALID_GIZMO_TYPE == currentGizmoType, buttonSize))
		{
			// currentGizmoType = INVALID_GIZMO_TYPE;
		}
		ImGui::SameLine(0, ImGuiUI::Padding);
		ImGuiUtils::DrawGizmoControlButton(ICON_FA_ARROWS_ALT, (int)ImGuizmo::OPERATION::TRANSLATE == currentGizmoType, buttonSize);
		{
			// currentGizmoType = (int)ImGuizmo::OPERATION::TRANSLATE;
		}
		ImGui::SameLine(0, ImGuiUI::Padding);
		ImGuiUtils::DrawGizmoControlButton(ICON_FA_SYNC_ALT, (int)ImGuizmo::OPERATION::ROTATE == currentGizmoType, buttonSize);
		{
			// currentGizmoType = (int)ImGuizmo::OPERATION::ROTATE;
		}
		ImGui::SameLine(0, ImGuiUI::Padding);
		ImGuiUtils::DrawGizmoControlButton(ICON_FA_EXPAND, (int)ImGuizmo::OPERATION::SCALE == currentGizmoType, buttonSize);
		{
			// currentGizmoType = (int)ImGuizmo::OPERATION::SCALE;
		}

		ImGui::End();

		ImGui::PopStyleVar(2);
	}

	bool ImGuiUtils::DrawGizmoControlButton(const char* icon, bool selected, ImVec2 buttonSize)
	{
		bool buttonPressed = false;

		static String gizmoTypeId = String("GizmoTypeButton");

		ImGui::PushID((gizmoTypeId + icon).c_str());

		if (selected)
		{
			// Selected
			ImGui::PushStyleColor(ImGuiCol_Button, ImGuiTheme::Enabled);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiTheme::Enabled);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiTheme::Enabled);
		}
		else
		{
			// Not selected
			ImGui::PushStyleColor(ImGuiCol_Button, ImGuiTheme::Disabled);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiTheme::Disabled);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiTheme::Disabled);
		}

		ImGui::Button(icon, buttonSize);
		// if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		// {
		// 	buttonPressed = true;
		// }

		ImGui::PopStyleColor(3);
		ImGui::PopID();

		return buttonPressed;
	}

} // namespace Mapo
