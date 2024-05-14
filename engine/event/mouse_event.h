//
// Created by Junhao Wang (@forkercat) on 5/13/24.
//

#pragma once

#include "engine/event/event.h"
#include "engine/input/mouse_codes.h"

namespace Mapo
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const F32 x, const F32 y)
			: m_mouseX(x), m_mouseY(y) { }

		F32 GetX() const { return m_mouseX; }
		F32 GetY() const { return m_mouseY; }

		virtual String ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << m_mouseX << ", " << m_mouseY;
			return ss.str();
		}

		MP_EVENT_CLASS_TYPE(MouseMoved)
		MP_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		F32 m_mouseX;
		F32 m_mouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const F32 offsetX, const F32 offsetY)
			: m_offsetX(offsetX), m_offsetY(offsetY)
		{
		}

		F32 GetOffsetX() const { return m_offsetX; }
		F32 GetOffsetY() const { return m_offsetY; }

		virtual String ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << GetOffsetX() << ", " << GetOffsetY();
			return ss.str();
		}

		MP_EVENT_CLASS_TYPE(MouseScrolled)
		MP_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		F32 m_offsetX, m_offsetY;
	};

	// Abstract base class
	class MouseButtonEvent : public Event
	{
	public:
		MouseCode GetMouseButton() const { return m_button; }

		MP_EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryInput)

	protected:
		MouseButtonEvent(const MouseCode button)
			: m_button(button)
		{
		}

		MouseCode m_button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseCode button)
			: MouseButtonEvent(button)
		{
		}

		virtual String ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << m_button;
			return ss.str();
		}

		MP_EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button)
			: MouseButtonEvent(button)
		{
		}

		virtual String ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << m_button;
			return ss.str();
		}

		MP_EVENT_CLASS_TYPE(MouseButtonReleased)
	};

} // namespace Mapo
