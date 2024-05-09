//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

#include "core/typedefs.h"
#include "engine/render/render_command.h"

namespace Mapo
{
	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(U32 width, U32 height);

		static void BeginScene(/* camera */);
		static void EndScene();

		static void Submit();

		static RendererAPI::GraphicsAPI GetAPI()
		{
			return RendererAPI::GetAPI();
		}
	};

} // namespace Mapo
