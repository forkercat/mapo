//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

#include "engine/render/render_context.h"

struct GLFWwindow;

namespace Mapo
{
	class VulkanRenderContext : public RenderContext
	{
	public:
		VulkanRenderContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_windowHandle;
	};

} // namespace Mapo
