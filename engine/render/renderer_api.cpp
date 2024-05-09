//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#include "renderer_api.h"

#include "core/uassert.h"
#include "platform/vulkan/vulkan_renderer_api.h"

namespace Mapo
{
	RendererAPI::GraphicsAPI RendererAPI::s_API = RendererAPI::GraphicsAPI::Vulkan;

	UniqueRef<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case RendererAPI::GraphicsAPI::None:
				MP_ASSERT(false, "RendererAPI::None is not supported!");
				return nullptr;
			case RendererAPI::GraphicsAPI::Vulkan:
				return MakeUnique<VulkanRendererAPI>();
		}

		MP_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
