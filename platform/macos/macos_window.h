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

		void OnUpdate() override;
		bool ShouldClose() const override;

		MP_FORCE_INLINE bool WasWindowResized() const override { return m_data.m_framebufferResized; }
		MP_FORCE_INLINE void ResetWindowResizedFlag() override { m_data.m_framebufferResized = false; }

		MP_FORCE_INLINE void* GetNativeWindow() const override { return m_window; }
		MP_FORCE_INLINE U32 GetWidth() const override { return m_data.width; }
		MP_FORCE_INLINE U32 GetHeight() const override { return m_data.height; }

		virtual void CreateWindowSurface(void* instance, void* surface) override;
		virtual void GlfwWaitEvents() override;
		virtual const char** GlfwGetRequiredExtensions(U32* count) override;

	private:
		void Init(const WindowProps& props);
		void Shutdown();

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void ErrorCallback(int error, const char* description);

	private:
		GLFWwindow* m_window = nullptr;

		struct WindowData
		{
			String title{};
			U32 width{};
			U32 height{};
			bool m_framebufferResized = false;
		};

		WindowData m_data{};
	};

} // namespace Mapo
