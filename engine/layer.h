//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#pragma once

#include "core/core.h"

#include "engine/event/event.h"

namespace Mapo
{
	class Layer
	{
	public:
		virtual ~Layer() = default;

		Layer(const String& debugName = "Layer");

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate(Timestep dt) { }
		virtual void OnImGuiRender() { }
		virtual void OnEvent(Event& event) { }

		const String& GetDebugName() const { return m_debugName; }

	protected:
		String m_debugName;
	};

} // namespace Mapo
