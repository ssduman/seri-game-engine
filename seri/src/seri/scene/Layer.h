#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace seri
{
	struct Layerable
	{
		Layerable(std::string layerName_) : layerName(layerName_) {}

		virtual ~Layerable() = default;

		virtual void Draw() = 0;

		std::string layerName;
	};

	class LayerManager
	{
	public:
		void AddGuiLayer(std::shared_ptr<Layerable> layer)
		{
			_guiLayer = layer;
		}

		void DeleteGuiLayer(std::shared_ptr<Layerable> layer)
		{
			_guiLayer.reset();
		}

		void AddDebugLayer(std::shared_ptr<Layerable> layer)
		{
			_debugLayer = layer;
		}

		void DeleteDebugLayer(std::shared_ptr<Layerable> layer)
		{
			_debugLayer.reset();
		}

		void AddUserLayer(std::shared_ptr<Layerable> layer)
		{
			_userLayer = layer;
		}

		void DeleteUserLayer(std::shared_ptr<Layerable> layer)
		{
			_userLayer.reset();
		}

		void AddSceneLayer(std::shared_ptr<Layerable> layer)
		{
			_sceneLayer = layer;
		}

		void DeleteSceneLayer(std::shared_ptr<Layerable> layer)
		{
			_sceneLayer.reset();
		}

		void Draw()
		{
			if (_userLayer)
			{
				_userLayer->Draw();
			}
			if (_sceneLayer)
			{
				_sceneLayer->Draw();
			}
			if (_debugLayer)
			{
				_debugLayer->Draw();
			}
			if (_guiLayer)
			{
				_guiLayer->Draw();
			}
		}

	private:

		std::shared_ptr<Layerable> _guiLayer;
		std::shared_ptr<Layerable> _userLayer;
		std::shared_ptr<Layerable> _debugLayer;
		std::shared_ptr<Layerable> _sceneLayer;

	};
}
