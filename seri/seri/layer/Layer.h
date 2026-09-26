#pragma once

#include "seri/profiling/Profiler.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace seri
{
	struct LayerBase
	{
		LayerBase(std::string layerName_) : layerName(layerName_) {}

		virtual ~LayerBase() = default;

		virtual void OnPreUpdate() {}
		virtual void OnUpdate() {}
		virtual void OnRender() {}
		virtual void OnPostUpdate() {}

		std::string layerName;
	};

	class LayerManager
	{
	public:
		void AddLayer(std::shared_ptr<LayerBase> layer)
		{
			if (std::find(_layers.begin(), _layers.end(), layer) == _layers.end())
			{
				_layers.emplace_back(std::move(layer));
			}
		}

		void DeleteLayer(const std::shared_ptr<LayerBase>& layer)
		{
			auto it = std::find(_layers.begin(), _layers.end(), layer);
			if (it != _layers.end())
			{
				_layers.erase(it);
			}
		}

		void OnPreUpdate()
		{
			SERI_PROFILER_ZONE_SCOPED;

			for (auto& layer : _layers)
			{
				SERI_PROFILER_ZONE_TRANSIENT(layer->layerName.c_str());

				layer->OnPreUpdate();
			}
		}

		void OnUpdate()
		{
			SERI_PROFILER_ZONE_SCOPED;

			for (auto& layer : _layers)
			{
				SERI_PROFILER_ZONE_TRANSIENT(layer->layerName.c_str());

				layer->OnUpdate();
			}
		}

		void OnRender()
		{
			SERI_PROFILER_ZONE_SCOPED;

			for (auto& layer : _layers)
			{
				SERI_PROFILER_ZONE_TRANSIENT(layer->layerName.c_str());

				layer->OnRender();
			}
		}

		void OnPostUpdate()
		{
			SERI_PROFILER_ZONE_SCOPED;

			for (auto& layer : _layers)
			{
				SERI_PROFILER_ZONE_TRANSIENT(layer->layerName.c_str());

				layer->OnPostUpdate();
			}
		}

	private:
		std::vector<std::shared_ptr<LayerBase>> _layers;

	};
}
