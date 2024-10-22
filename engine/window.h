//
// Created by Junhao Wang (@forkercat) on 5/6/24.
//

#pragma once

#include "core/typedefs.h"
#include "core/string/string.h"

#include "engine/event/event.h"

namespace Mapo
{
	// Window creation data.
	struct WindowProps
	{
		String title;
		U32	   width;
		U32	   height;

		WindowProps(const String& title_ = "Hello Window", U32 width_ = 800, U32 height_ = 600)
			: title(title_), width(width_), height(height_)
		{
		}
	};

	// Platform independent class.
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		Window() = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		virtual void OnUpdate() = 0;
		virtual bool WasFramebufferResized() const = 0;
		virtual void ResetFramebufferResizedFlag() = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual U32 GetWidth() const = 0;
		virtual U32 GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		// For Vulkan and GLFW.
		virtual void		 CreateWindowSurface(void* instance, void* surface) = 0;
		virtual void		 GlfwWaitEvents() = 0;
		virtual const char** GlfwGetRequiredExtensions(U32* count) = 0;
		virtual void*		 GetCurrentContext() = 0;
		virtual void		 MakeCurrentContext(void* context) = 0;

		static UniqueRef<Window> Create(const WindowProps& props = {});
	};

} // namespace Mapo
