//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

#include "core/typedefs.h"

namespace Mapo
{
	class RendererAPI
	{
	public:
		enum class GraphicsAPI
		{
			None = 0,
			Vulkan,
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(U32 x, U32 y, U32 width, U32 height) = 0;
		virtual void SetClearColor() = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(/* Vertex Array */) = 0;

		static GraphicsAPI GetAPI() { return s_API; }
		static UniqueRef<RendererAPI> Create();

	private:
		static GraphicsAPI s_API;
	};

} // namespace Mapo
