//
// Created by Junhao Wang (@forkercat) on 5/13/24.
//

#pragma once

#include "engine/event/event.h"

namespace Mapo
{
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(U32 width, U32 height)
			: m_width(width), m_height(height)
		{
		}

		U32 GetWidth() const { return m_width; }
		U32 GetHeight() const { return m_height; }

		String ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << m_width << " x " << m_height;
			return ss.str();
		}

		MP_EVENT_CLASS_TYPE(WindowResize)
		MP_EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		U32 m_width;
		U32 m_height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		MP_EVENT_CLASS_TYPE(WindowClose)
		MP_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	/////////////////////////////////////////////////////////////////////////////////

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		MP_EVENT_CLASS_TYPE(AppTick)
		MP_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		MP_EVENT_CLASS_TYPE(AppUpdate)
		MP_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		MP_EVENT_CLASS_TYPE(AppRender)
		MP_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

} // namespace Mapo
