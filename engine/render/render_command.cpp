//
// Created by Junhao Wang (@forkercat) on 5/8/24.
//

#include "render_command.h"

namespace Mapo
{
	UniqueRef<RendererAPI> RenderCommand::s_rendererAPI = RendererAPI::Create();

} // namespace Mapo
