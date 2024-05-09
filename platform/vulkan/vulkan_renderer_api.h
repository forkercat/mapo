//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#pragma once

#include "engine/render/renderer_api.h"

namespace Mapo
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(U32 x, U32 y, U32 width, U32 height) override;
		virtual void SetClearColor() override;
		virtual void Clear() override;

		virtual void DrawIndexed(/* Vertex Array */) override;
	};

} // namespace Mapo
