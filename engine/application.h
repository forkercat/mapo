//
// Created by Junhao Wang (@forkercat) on 5/4/24.
//

#pragma once

#include "core/core.h"

#include "engine/window.h"
#include "engine/layer_stack.h"
#include "engine/ui/imgui_layer.h"

int MapoMain(int argc, char** argv);

namespace Mapo
{
	class RenderContext;

	struct ApplicationCommandLineArgs
	{
		int count = 0;
		char** args = nullptr;

		const char* operator[](int index) const
		{
			return args[index];
		}
	};

	// Application base class
	class Application
	{
	public:
		virtual ~Application();

		Application(const String& name = "Mapo App", ApplicationCommandLineArgs args = {});

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		MP_FORCE_INLINE static Application& Get() { return *s_appInstance; }
		MP_FORCE_INLINE Window& GetWindow() { return *m_window; }
		MP_FORCE_INLINE ApplicationCommandLineArgs GetCommandLineArgs() const { return m_commandLineArgs; }

		virtual bool Start();
		void Close() { m_running = false; }

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	private:
		// Subclass cannot override.
		void Run();

		// Event callbacks.
		void OnEvent() { }
		void OnWindowClose() { }
		bool OnEscPressed() { return false; }
		bool OnWindowResize() { return false; }

	public:
		static constexpr U32 WIDTH = 960;
		static constexpr U32 HEIGHT = 600;

	private:
		ApplicationCommandLineArgs m_commandLineArgs;

		UniqueRef<Window> m_window;

		LayerStack* m_layerStack;
		ImGuiLayer* m_imguiLayer = nullptr;

		bool m_running = true;
		bool m_minimalized = false;

		Timer m_timer;

		// Holds one application instance.
		static Application* s_appInstance;

	private:
		// MapoMain then can call the private Run function.
		friend int ::MapoMain(int argc, char** argv);
	};

	// This will be defined by client.
	Application* CreateApplication(ApplicationCommandLineArgs args);

} // namespace Mapo
