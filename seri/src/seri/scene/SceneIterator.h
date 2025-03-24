#pragma once

#include "seri/scene/Scene.h"

#include <memory>
#include <vector>

struct SceneIterator
{
	SceneIterator(std::shared_ptr<IScene> scene)
	{
		prepareDrawVector(std::move(scene));
	}

	auto getDrawVector()
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
	void prepareDrawVector(const std::shared_ptr<IScene>& scene)
	{
		_drawVector.push_back(scene);

		for (const auto& s : scene->getChildren())
		{
			prepareDrawVector(s);
		}
	}

	std::vector<std::shared_ptr<IScene>> _drawVector;

};
