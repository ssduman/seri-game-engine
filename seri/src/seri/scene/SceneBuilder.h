#pragma once

#include "seri/scene/Scene.h"

#include <memory>
#include <string>

namespace seri
{
	struct SceneBuilder
	{
		SceneBuilder()
		{
			_scene = std::make_shared<SceneComponent>("");
		}

		SceneBuilder& SetName(std::string name)
		{
			_scene->SetName(std::move(name));
			return *this;
		}

		SceneBuilder& SetUnderlyingObject(std::shared_ptr<Object> object)
		{
			_scene->SetUnderlyingObject(std::move(object));
			return *this;
		}

		SceneBuilder& Add(std::shared_ptr<IScene> child)
		{
			_scene->Add(std::move(child));
			return *this;
		}

		std::shared_ptr<IScene> Build()
		{
			auto scene = _scene;
			Make();
			return scene;
		}

		SceneBuilder& Make()
		{
			_scene = std::make_shared<SceneComponent>("");
			return *this;
		}

	private:
		std::shared_ptr<IScene> _scene;

	};
}
