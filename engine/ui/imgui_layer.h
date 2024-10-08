//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#pragma once

#include "engine/layer.h"

namespace Mapo
{
	class Device;

	class ImGuiLayer : public Layer
	{
	public:
		virtual ~ImGuiLayer() = default;

		ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin();
		virtual void End();

		virtual void OnImGuiRender() override;

		void RunExample();

		virtual void OnEvent(Event& event) override;

		void BlockEvents(bool block) { m_blockEvents = block; }

		void SetStyles();

	public:
		bool showDemoWindow = true;
		bool showAnotherWindow = false;

	private:
		Device& m_device;

		bool m_blockEvents = false;
	};

} // namespace Mapo
