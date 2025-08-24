#pragma once

#include "seri/scene/Scene.h"

#include <memory>
#include <vector>

namespace seri
{
	struct SceneIterator
	{
		SceneIterator(std::shared_ptr<IScene> scene)
		{
			PrepareDrawVector(std::move(scene));
		}

		auto GetDrawVector()
		{
			return _drawVector;
		}

		auto begin()
		{
			return _drawVector.rbegin();
		}

		auto end()
		{
			return _drawVector.rend();
		}

	private:
		void PrepareDrawVector(const std::shared_ptr<IScene>& scene)
		{
			_drawVector.push_back(scene);

			for (const auto& s : scene->GetChildren())
			{
				PrepareDrawVector(s);
			}
		}

		std::vector<std::shared_ptr<IScene>> _drawVector;

	};
}
