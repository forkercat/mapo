//
// Created by Junhao Wang (@forkercat) on 5/4/24.
//

#include "application.h"

#include "engine/renderer/render_context.h"
#include "engine/renderer/device.h"

namespace Mapo
{
	Application* Application::s_appInstance = nullptr;

	Application::Application(const String& name, ApplicationCommandLineArgs args)
		: m_commandLineArgs(args)
	{
		// Self assign.
		s_appInstance = this;

		// Create a window and the render context.
		m_window = Window::Create(WindowProps(name, WIDTH, HEIGHT));

		RenderContext::Init();

		m_imguiLayer = MP_NEW(ImGuiLayer);
		PushOverlay(m_imguiLayer);
	}

	Application::~Application()
	{
		RenderContext::Release();
	}

	bool Application::Start()
	{
		MP_ASSERT(RenderContext::IsInitialized(), "Render context is not initialized!");

		return true;
	}

	void Application::Run()
	{
		m_timer.Start();

		while (!m_window->ShouldClose())
		{
			Timestep deltaTime = static_cast<Timestep>(m_timer.Tick());

			if (!m_minimalized)
			{
				// Update
				for (Layer* layer : m_layerStack)
				{
					layer->OnUpdate(deltaTime);
				}

				// ImGui
				m_imguiLayer->Begin();
				for (Layer* layer : m_layerStack)
				{
					layer->OnImGuiRender();
				}
				m_imguiLayer->End();
			}

			m_window->OnUpdate();
		}

		RenderContext::GetDevice().WaitIdle();
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Layers
	/////////////////////////////////////////////////////////////////////////////////

	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

} // namespace Mapo
