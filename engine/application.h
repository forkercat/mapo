//
// Created by Junhao Wang (@forkercat) on 5/4/24.
//

#pragma once

#include "core/core.h"
#include "engine/scene.h"
#include "engine/render/vulkan_device.h"
#include "engine/render/vulkan_renderer.h"
#include "engine/render/vulkan_buffer.h"
#include "engine/render/vulkan_descriptors.h"

int MapoMain(int argc, char** argv);

namespace Mapo
{
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
		Application(const String& name = "Mapo App", ApplicationCommandLineArgs args = {});
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		ApplicationCommandLineArgs GetCommandLineArgs() const
		{
			return m_commandLineArgs;
		}

		// TODO: Hold window!

		virtual bool Start();
		void Close() {}

	private:
		// Subclass cannot override.
		void Run();

	public:
		static constexpr U32 WIDTH = 960;
		static constexpr U32 HEIGHT = 600;

	private:
		ApplicationCommandLineArgs m_commandLineArgs;

		VulkanWindow m_window{ WIDTH, HEIGHT, "Hello Vulkan!" };
		VulkanDevice m_device{ m_window };
		VulkanRenderer m_renderer{ m_window, m_device };

		// Note: Order of declarations matters.
		UniqueRef<VulkanDescriptorPool> m_globalDescriptorPool{};

		UniqueRef<Scene> m_scene;

		// Holds one application instance.
		static Application* s_appInstance;

	private:
		// MapoMain then can call the private Run function.
		friend int ::MapoMain(int argc, char** argv);
	};

	// This will be defined by client.
	Application* CreateApplication(ApplicationCommandLineArgs args);

} // namespace Mapo
