//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#pragma once

#include "engine/layer.h"

namespace Mapo
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent() override;

		virtual void Begin();
		virtual void End();

		void BlockEvents(bool block) { m_blockEvents = block; }

		void SetDarkThemeColors();

	private:
		bool m_blockEvents = true;
	};

} // namespace Mapo
