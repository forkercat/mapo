//
// Created by Junhao Wang (@forkercat) on 5/6/24.
//

#pragma once

#include "engine/window.h"

class GLFWwindow;

namespace Mapo
{
	class MacosWindow : public Window
	{
	public:
		virtual ~MacosWindow();

		MacosWindow(const WindowProps& props);

		virtual void OnUpdate() override;

		virtual bool  WasFramebufferResized() const override { return m_data.framebufferResized; }
		virtual void  ResetFramebufferResizedFlag() override { m_data.framebufferResized = false; }
		virtual void* GetNativeWindow() const override { return m_window; }
		virtual U32	  GetWidth() const override { return m_data.width; }
		virtual U32	  GetHeight() const override { return m_data.height; }

		virtual void SetEventCallback(const EventCallbackFn& callback) override;

		virtual void		 CreateWindowSurface(void* instance, void* surface) override;
		virtual void		 GlfwWaitEvents() override;
		virtual const char** GlfwGetRequiredExtensions(U32* count) override;
		virtual void*		 GetCurrentContext() override;
		virtual void		 MakeCurrentContext(void* context) override;

	private:
		void Init(const WindowProps& props);
		void Shutdown();

		// Callbacks
		static void WindowResizeCallback(GLFWwindow* window, int width, int height);
		static void WindowCloseCallback(GLFWwindow* window);
		static void KeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void CharEventCallback(GLFWwindow* window, unsigned int keycode);
		static void MouseEventCallback(GLFWwindow* window, int button, int action, int mods);
		static void ScrollEventCallback(GLFWwindow* window, double offsetX, double offsetY);
		static void CursorEventCallback(GLFWwindow* window, double posX, double posY);

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void ErrorCallback(int error, const char* description);

	private:
		GLFWwindow* m_window = nullptr;

		struct WindowData
		{
			String			title{};
			U32				width{};
			U32				height{};
			bool			framebufferResized = false;
			EventCallbackFn eventCallback;
		};

		WindowData m_data{};
	};

} // namespace Mapo
