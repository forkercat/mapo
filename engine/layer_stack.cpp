//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#include "layer_stack.h"

namespace Mapo
{
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_layers)
		{
			layer->OnDetach();
			MP_DELETE(layer, StdAllocator::Get());
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		auto iter = std::find(m_layers.begin(), m_layers.end(), layer);
		MP_ASSERT(iter == m_layers.end(), "Same layer could not be added to stack twice!");

		m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
		m_layerInsertIndex++;
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto iter = std::find(m_layers.begin(), m_layers.end(), layer);

		if (iter != m_layers.end())
		{
			m_layers.erase(iter);
			m_layerInsertIndex--;
		}
		else
		{
			MP_WARN("The layer being popped is not present! Skip.");
		}
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		auto iter = std::find(m_layers.begin(), m_layers.end(), overlay);
		MP_ASSERT(iter == m_layers.end(), "Same overlay could not be added to stack twice!");

		m_layers.emplace_back(overlay);
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto iter = std::find(m_layers.begin(), m_layers.end(), overlay);

		if (iter != m_layers.end())
		{
			m_layers.erase(iter);
		}
		else
		{
			MP_WARN("The overlay being popped is not present! Skip.");
		}
	}

} // namespace Mapo
