//
// Created by Junhao Wang (@forkercat) on 5/4/24.
//

#include "application.h"

#include "engine/renderer/render_context.h"
#include "engine/renderer/device.h"
#include "engine/renderer/renderer.h"

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
		m_window->SetEventCallback(MP_BIND_EVENT_FN(Application::OnEvent));
		// Alternatively you can just pass in the application ptr.

		RenderContext::Init();

		// Layers
		m_layerStack = MP_NEW(LayerStack);

		m_imguiLayer = MP_NEW(ImGuiLayer);
		PushOverlay(m_imguiLayer);
	}

	Application::~Application()
	{
		MP_DELETE(m_layerStack);

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

		while (m_running)
		{
			Timestep deltaTime = static_cast<Timestep>(m_timer.Tick());

			if (!m_minimalized)
			{
				Renderer& renderer = RenderContext::GetRenderer();

				// Could be nullptr if, for example, the swapchain needs to be recreated.
				if ([[maybe_unused]] VkCommandBuffer commandBuffer = renderer.BeginFrame())
				{
					// The reason why BeginFrame and BeginRenderPass are separate functions is
					// we want the app to control over this to enable us easily integrating multiple render passes.
					//
					// - BeginFrame to acquire image and begin command buffer
					// - Begin offscreen shadow pass
					// -   Render shadow casting objects
					// - End offscreen shadow pass
					// - Begin shading pass
					// -   Render objects
					// - End shading pass
					// - Post processing...
					renderer.BeginRenderPass();

					// TODO: Render pass should be put inside!

					// Update
					for (Layer* layer : *m_layerStack)
					{
						layer->OnUpdate(deltaTime);
					}

					// ImGui
					m_imguiLayer->Begin();

					for (Layer* layer : *m_layerStack)
					{
						layer->OnImGuiRender();
					}

					m_imguiLayer->End();

					renderer.EndRenderPass();

					renderer.EndFrame();
				}
			}

			m_window->OnUpdate(); // glfwPollEvents()
		}

		RenderContext::GetDevice().WaitIdle();
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Events
	/////////////////////////////////////////////////////////////////////////////////

	void Application::OnEvent(Event& event)
	{
		// Handles events received from window.
		EventDispatcher dispatcher(event);

		// Dispatcher will check if event is matched.
		dispatcher.Dispatch<WindowCloseEvent>(MP_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(MP_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<KeyPressedEvent>(MP_BIND_EVENT_FN(Application::OnEscPressed));

		// Handles other events in each layer.
		for (auto iter = m_layerStack->rbegin(); iter != m_layerStack->rend(); ++iter)
		{
			if (event.handled)
			{
				break;
			}

			(*iter)->OnEvent(event);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_running = false;
		return true;
	}

	bool Application::OnEscPressed(KeyPressedEvent& event)
	{
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		// MP_INFO("Event (WindowResize): Resize to [{}, {}]", event.GetWidth(), event.GetHeight());

		if (event.GetWidth() == 0 || event.GetHeight() == 0)
		{
			m_minimalized = true;
			return false;
		}

		m_minimalized = false;

		// Note that this is not needed because it is handled by framebuffer resize.
		// Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Layers
	/////////////////////////////////////////////////////////////////////////////////

	void Application::PushLayer(Layer* layer)
	{
		m_layerStack->PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_layerStack->PushOverlay(overlay);
		overlay->OnAttach();
	}

} // namespace Mapo
