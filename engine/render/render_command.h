//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

#include "core/typedefs.h"
#include "engine/render/renderer_api.h"

namespace Mapo
{
	class RenderCommand
	{
	public:
		static void Init()
		{
			s_rendererAPI->Init();
		}

		static void SetViewport(U32 x, U32 y, U32 width, U32 height)
		{
			s_rendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor()
		{
			s_rendererAPI->SetClearColor();
		}

		static void Clear()
		{
			s_rendererAPI->Clear();
		}

		static void DrawIndexed()
		{
			s_rendererAPI->DrawIndexed();
		}

	private:
		static UniqueRef<RendererAPI> s_rendererAPI;
	};

} // namespace Mapo
