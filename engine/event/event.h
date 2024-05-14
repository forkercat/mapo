//
// Created by Junhao Wang (@forkercat) on 5/13/24.
//

#pragma once

#include "core/core.h"

namespace Mapo
{
	enum class EventType
	{
		// clang-format off
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
		// clang-format on
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

	/////////////////////////////////////////////////////////////////////////////////

	class Event
	{
		friend class EventDispatcher;

	public:
		virtual ~Event() = default;

		virtual EventType	GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int			GetCategoryFlags() const = 0;
		virtual String		ToString() const { return GetName(); }

		bool InCategory(EventCategory category) { return GetCategoryFlags() & category; }

		bool handled = false;
	};

#define MP_EVENT_CLASS_TYPE(type)                   \
	static EventType GetStaticType()                \
	{                                               \
		return EventType::type;                     \
	}                                               \
	virtual EventType GetEventType() const override \
	{                                               \
		return GetStaticType();                     \
	}                                               \
	virtual const char* GetName() const override    \
	{                                               \
		return #type;                               \
	}

#define MP_EVENT_CLASS_CATEGORY(category)         \
	virtual int GetCategoryFlags() const override \
	{                                             \
		return category;                          \
	}

// #define MP_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define MP_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

	/////////////////////////////////////////////////////////////////////////////////

	class EventDispatcher
	{
		template <typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		virtual ~EventDispatcher() = default;

		EventDispatcher(Event& event)
			: m_event(event)
		{
		}

		// Generated for each type of events. Return true if handled.
		template <typename T>
		bool Dispatch(EventFn<T> fn)
		{
			if (m_event.GetEventType() == T::GetStaticType())
			{
				m_event.handled |= fn(*static_cast<T*>(&m_event));
				return true;
			}

			return false;
		}

	private:
		Event& m_event;
	};

	inline std::ostream& operator<<(std::ostream&& os, const Event& event)
	{
		return os << event.ToString();
	}

} // namespace Mapo
