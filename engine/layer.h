//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#pragma once

#include "core/core.h"

namespace Mapo
{
	class Layer
	{
	public:
		Layer(const String& debugName = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate(Timestep dt) { }
		virtual void OnImGuiRender() { }
		virtual void OnEvent(/*Event*/) { }

		const String& GetDebugName() const { return m_debugName; }

	protected:
		String m_debugName;
	};

} // namespace Mapo
