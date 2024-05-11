//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

#include "core/core.h"

namespace Mapo
{
	class Device;
	class Renderer;
	class DescriptorPool;

	class RenderContext final
	{
	public:
		virtual ~RenderContext() = default;

		static void Init();
		static void Release();
		static bool IsInitialized() { return s_context != nullptr; }
		static void SwapBuffers();

		static Device& GetDevice() { return *s_context->m_device; }
		static DescriptorPool& GetDescriptorPool() { return *s_context->m_descriptorPool; }

	private:
		RenderContext() = default;

		RenderContext(const RenderContext&) = delete;
		RenderContext(RenderContext&&) = delete;
		RenderContext& operator=(const RenderContext&) = delete;
		RenderContext& operator=(RenderContext&&) = delete;

	private:
		// Vulkan resources.
		UniqueRef<Device> m_device{};
		UniqueRef<DescriptorPool> m_descriptorPool{};

		static RenderContext* s_context;
	};

} // namespace Mapo
