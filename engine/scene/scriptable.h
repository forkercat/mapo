//
// Created by Junhao Wang (@forkercat) on 5/17/24.
//

#pragma once

#include "engine/scene/game_object.h"

namespace Mapo
{
	class Scriptable
	{
	public:
		~Scriptable() = default;

		Scriptable(const String& scriptName)
			: m_scriptName(scriptName) { }

		template <typename T>
		T& GetComponent()
		{
			return m_gameObject.GetComponent<T>();
		}

		virtual const String& GetScriptName() const { return m_scriptName; }

	protected:
		virtual void OnCreate() { }
		virtual void OnDestroy() { }
		virtual void OnUpdateEditor(Timestep dt) { }
		virtual void OnUpdateRuntime(Timestep dt) { }

	private:
		GameObject m_gameObject;

		String m_scriptName{};

		friend class Scene;
		friend struct NativeScriptComponent;
	};

} // namespace Mapo
