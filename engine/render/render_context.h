//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

#include "core/core.h"

namespace Mapo
{
	class RenderContext
	{
	public:
		virtual ~RenderContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static UniqueRef<RenderContext> Create(void* window);
	};

} // namespace Mapo
