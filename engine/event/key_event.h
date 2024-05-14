//
// Created by Junhao Wang (@forkercat) on 5/13/24.
//

#pragma once

#include "engine/event/event.h"
#include "engine/input/key_codes.h"

namespace Mapo
{
	// Abstract key event.
	class KeyEvent : public Event
	{
	public:
		virtual ~KeyEvent() = default;

		KeyCode GetKeyCode() const { return m_keyCode; }

		MP_EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(const KeyCode code)
			: m_keyCode(code) { }

		KeyCode m_keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode code, const U16 repeatCount)
			: KeyEvent(code), m_repeatCount(repeatCount)
		{
		}

		U16 GetRepeatCount() const { return m_repeatCount; }

		virtual String ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << m_keyCode << " (" << m_repeatCount << " repeats)";
			return ss.str();
		}

		MP_EVENT_CLASS_TYPE(KeyPressed)

	private:
		U16 m_repeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode code)
			: KeyEvent(code)
		{
		}

		virtual String ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << m_keyCode;
			return ss.str();
		}

		MP_EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode code)
			: KeyEvent(code)
		{
		}

		virtual String ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << m_keyCode;
			return ss.str();
		}

		MP_EVENT_CLASS_TYPE(KeyTyped)
	};

} // namespace Mapo
