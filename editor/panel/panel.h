//
// Created by Junhao Wang (@forkercat) on 5/18/24.
//

#pragma once

#include "core/core.h"

namespace Mapo
{
	class Panel
	{
	public:
		virtual ~Panel() = default;

		Panel(const String& panelName)
			: m_panelName(panelName) { }

		const String& GetPanelName() const { return m_panelName; }

	protected:
		String m_panelName = "Unnamed Panel";
	};

} // namespace Mapo
