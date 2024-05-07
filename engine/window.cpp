//
// Created by Junhao Wang (@forkercat) on 5/6/24.
//

#include "window.h"

#ifdef MP_MACOS_BUILD
	#include "platform/macos/macos_window.h"
#endif

namespace Mapo
{
	UniqueRef<Window> Window::Create(const WindowProps& props)
	{
#ifdef MP_MACOS_BUILD
		return MakeUnique<MacosWindow>(props);
#else
		ASSERT(false, "Unsupported platform!");
		return nullptr;
#endif
	}

} // namespace Mapo
