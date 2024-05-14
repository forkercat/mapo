//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#include "render_context.h"

#include "engine/window.h"
#include "engine/application.h"

#include "engine/renderer/device.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/swapchain.h"
#include "engine/renderer/descriptors.h"

namespace Mapo
{
	RenderContext* RenderContext::s_context = nullptr;

	void RenderContext::Init()
	{
		MP_ASSERT(s_context == nullptr, "The context has already been initialized. You cannot initialize it twice!");

		Window& window = Application::Get().GetWindow();

		s_context = MP_NEW(RenderContext);

		s_context->m_device = MakeUnique<Device>(window);

		s_context->m_renderer = MakeUnique<Renderer>();

		s_context->m_descriptorPool =
			DescriptorPool::Builder()
				// How many descriptor sets can be created from the pool.
				.SetMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
				// How many descriptors of this type are available in the pool.
				.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT)
				.Build();
	}

	void RenderContext::Release()
	{
		MP_ASSERT(s_context, "The context instance is nullptr!");
		MP_DELETE(s_context);
	}

	void RenderContext::SwapBuffers()
	{
		//
	}

	U32 RenderContext::GetMaxFramesInFlight()
	{
		return Swapchain::MAX_FRAMES_IN_FLIGHT;
	}

} // namespace Mapo
