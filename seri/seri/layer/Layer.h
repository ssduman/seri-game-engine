#pragma once

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

		virtual void PreUpdate() {}
		virtual void Update() {}
		virtual void OnRender() {}
		virtual void PostUpdate() {}

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

		void PreUpdate()
		{
			for (auto& layer : _layers)
			{
				layer->PreUpdate();
			}
		}

		void Update()
		{
			for (auto& layer : _layers)
			{
				layer->Update();
			}
		}

		void OnRender()
		{
			for (auto& layer : _layers)
			{
				layer->OnRender();
			}
		}

		void PostUpdate()
		{
			for (auto& layer : _layers)
			{
				layer->PostUpdate();
			}
		}

	private:
		std::vector<std::shared_ptr<LayerBase>> _layers;

	};
}
