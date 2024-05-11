//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#pragma once

#include "engine/layer.h"

namespace Mapo
{
	// List: [ <--- layers --  --- overlays ---> ]
	class LayerStack final
	{
	public:
		LayerStack();
		virtual ~LayerStack();

		// Push layers into the first half of the list.
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		// Push layers into the second half of the list.
		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		// Iterators
		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }
		std::vector<Layer*>::reverse_iterator rbegin() { return m_layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_layers.rend(); }

		std::vector<Layer*>::const_iterator begin() const { return m_layers.begin(); }
		std::vector<Layer*>::const_iterator end() const { return m_layers.end(); }
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend() const { return m_layers.rend(); }

	private:
		std::vector<Layer*> m_layers;
		U32 m_layerInsertIndex = 0;
	};

} // namespace Mapo
