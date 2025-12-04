#include "Seripch.h"

#include "seri/scene/Scene.h"
#include "seri/scene/SceneManager.h"
#include "seri/util/Util.h"
#include "seri/texture/Skybox.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

#include <string>
#include <memory>
#include <unordered_map>

namespace seri::scene
{
	void seri::scene::Scene::Deserialize(const std::string& file)
	{
		YAML::Node root = YAML::LoadFile(file);

		if (!root["Scene"] || !root["Scene"].IsMap())
		{
			LOGGER(error, fmt::format("[scene] failed to deserialize scene from file: {}", file));
			return;
		}

		YAML::Node sceneNode = root["Scene"];

		_idComponent = seri::IDComponent::Deserialize(sceneNode["IDComponent"]);
		_sceneComponent = seri::SceneComponent::Deserialize(sceneNode["SceneComponent"]);

		_entityMap.clear();

		if (YAML::Node entities = sceneNode["Entities"])
		{
			for (auto entityItem : entities)
			{
				YAML::Node entityData = entityItem["Entity"];

				for (auto componentNode : entityData)
				{
					std::string componentName = componentNode.first.as<std::string>();
					YAML::Node componentData = componentNode.second;

					entt::entity entity = seri::scene::SceneManager::CreateEntity();

					if (componentName == "IDComponent")
					{
						seri::IDComponent idComp = seri::IDComponent::Deserialize(componentData);
					}
					else if (componentName == "TransformComponent")
					{
						seri::TransformComponent transformComp = seri::TransformComponent::Deserialize(componentData);
					}
					else if (componentName == "SceneComponent")
					{
						seri::SceneComponent sceneComp = seri::SceneComponent::Deserialize(componentData);
					}
					else
					{
						LOGGER(error, fmt::format("[scene] unknown component type: {}", componentName));
					}
				}
			}
		}
	}
}
