//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#include "render_context.h"

#include "core/uassert.h"

#include "engine/window.h"
#include "engine/render/renderer.h"

#include "platform/vulkan/vulkan_render_context.h"

namespace Mapo
{
	UniqueRef<RenderContext> RenderContext::Create(Window& window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::GraphicsAPI::None:
				MP_ASSERT(false, "Graphics API is none!");
				return nullptr;
			case RendererAPI::GraphicsAPI::Vulkan:
				return MakeUnique<VulkanRenderContext>(window);
		}

		MP_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

} // namespace Mapo
