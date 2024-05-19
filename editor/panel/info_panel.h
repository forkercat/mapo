//
// Created by Junhao Wang (@forkercat) on 5/19/24.
//

#pragma once

#include "editor/panel/panel.h"

namespace Mapo
{
	class InfoPanel : public Panel
	{
	public:
		virtual ~InfoPanel() = default;

		InfoPanel();

		void OnImGuiRender();
	};
}
