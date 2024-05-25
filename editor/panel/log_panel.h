//
// Created by Junhao Wang (@forkercat) on 5/25/24.
//

#pragma once

#include "editor/panel/panel.h"

namespace Mapo
{
	class LogPanel : public Panel
	{
	public:
		virtual ~LogPanel() = default;

		LogPanel();

		void OnImGuiRender();
	};
}
