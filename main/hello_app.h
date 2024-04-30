//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#pragma once

#include "core/core.h"

#include "engine/vulkan_window.h"
#include "engine/game_object.h"
#include "engine/camera.h"

#include "engine/vulkan_device.h"
#include "engine/vulkan_renderer.h"
#include "engine/vulkan_buffer.h"
#include "engine/vulkan_descriptors.h"

#include <vector>
#include <memory>

namespace mapo
{
	class HelloApp
	{
	public:
		HelloApp();
		~HelloApp();

		HelloApp(const HelloApp&) = delete;
		HelloApp& operator=(const HelloApp&) = delete;

		void Run();

	public:
		static constexpr U32 WIDTH = 960;
		static constexpr U32 HEIGHT = 600;

	private:
		void LoadGameObjects();

	private:
		VulkanWindow m_window{ WIDTH, HEIGHT, "Hello Vulkan!" };
		VulkanDevice m_device{ m_window };
		VulkanRenderer m_renderer{ m_window, m_device };

		// Note: Order of declarations matters.
		UniqueRef<VulkanDescriptorPool> m_globalDescriptorPool{};

		GameObject::Map m_gameObjects;
	};

} // namespace mapo
