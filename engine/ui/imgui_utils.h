//
// Created by Junhao Wang (@forkercat) on 5/23/24.
//

#pragma once

#include "core/core.h"

#include <imgui/imgui.h>

namespace Mapo
{
	struct ImGuiUtils
	{
		static void DrawControlFloat3(const char* label, Vector3& values, const char* format = "%.1f", F32 resetValue = 0.0f, F32 columnWidth = 100.0f);
		static void DrawGizmoControls(int& currentGizmoType);
		static bool DrawGizmoControlButton(const char* icon, bool selected, ImVec2 buttonSize);
	};

	namespace ImGuiConfig
	{
		static F32 Dpi = 2.0f;
	}

	namespace ImGuiTheme
	{
		// static ImColor MainTheme{ 214, 166, 48, 255 };  // yellow
		static ImU32 MainTheme = IM_COL32(46, 110, 184, 255); // blue

		static ImU32 ComponentX = IM_COL32(204, 90, 90, 255);
		static ImU32 ComponentHoveredX = IM_COL32(230, 90, 90, 255);

		static ImU32 ComponentY = IM_COL32(90, 204, 90, 255);
		static ImU32 ComponentHoveredY = IM_COL32(102, 217, 102, 255);

		static ImU32 ComponentZ = IM_COL32(90, 143, 194, 255);
		static ImU32 ComponentHoveredZ = IM_COL32(102, 166, 230, 255);

		// Console
		static ImU32 ConsoleDebug = IM_COL32(102, 230, 102, 255);
		static ImU32 ConsoleInfo = IM_COL32(230, 230, 230, 255);
		static ImU32 ConsoleWarn = IM_COL32(230, 230, 102, 255);
		static ImU32 ConsoleError = IM_COL32(230, 102, 102, 255);

		// General
		static ImU32 WindowBg = IM_COL32(38, 38, 38, 255);
		static ImU32 PopupBg = IM_COL32(38, 38, 38, 242);
		static ImU32 Text = ConsoleInfo;

		static ImU32 Normal = IM_COL32(51, 51, 51, 255);
		static ImU32 Hovered = IM_COL32(77, 77, 77, 255);
		static ImU32 Active = IM_COL32(38, 38, 38, 255);

		static ImU32 Enabled = IM_COL32(77, 77, 77, 255);
		static ImU32 Disabled = IM_COL32(51, 51, 51, 255);

		static ImU32 TabNormal = IM_COL32(38, 38, 38, 255);
		static ImU32 TabHovered = IM_COL32(97, 97, 97, 255);
		static ImU32 TabActive = IM_COL32(71, 71, 71, 255);
		static ImU32 TabUnfocused = IM_COL32(38, 38, 38, 255);
		static ImU32 TabUnfocusedActive = IM_COL32(51, 51, 51, 255);

		static ImU32 TitleBg = IM_COL32(38, 38, 38, 255);
		static ImU32 TitleBgActive = IM_COL32(38, 38, 38, 255);
		static ImU32 TitleBgCollapsed = IM_COL32(38, 38, 38, 255);

		static ImU32 Separator = IM_COL32(60, 60, 60, 128);
		static ImU32 SeparatorHovered = IM_COL32(60, 60, 60, 204);
		static ImU32 SeparatorActive = IM_COL32(60, 60, 60, 204);

		static ImU32 ScrollBarBg = WindowBg;

	} // namespace ImGuiTheme

} // namespace Mapo
