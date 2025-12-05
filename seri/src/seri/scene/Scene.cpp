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

		_sceneGraphRoot = GraphNode{};
		_entityMap.clear();
		auto& registry = seri::scene::SceneManager::GetRegistry();

		if (YAML::Node entities = sceneNode["Entities"])
		{
			for (auto entityItem : entities)
			{
				YAML::Node entityData = entityItem["Entity"];

				entt::entity entity = seri::scene::SceneManager::CreateEntity();

				seri::IDComponent idComp = seri::IDComponent::Deserialize(entityData["IDComponent"]);
				registry.emplace<seri::IDComponent>(entity, idComp);

				FindAndAddAsChild(_sceneGraphRoot, idComp.parentId, idComp);

				_entityMap[idComp.id] = entity;

				for (auto componentNode : entityData)
				{
					std::string componentName = componentNode.first.as<std::string>();
					YAML::Node componentData = componentNode.second;

					if (componentName == "IDComponent")
					{
					}
					else if (componentName == "TransformComponent")
					{
						seri::TransformComponent transformComp = seri::TransformComponent::Deserialize(componentData);
						registry.emplace<seri::TransformComponent>(entity, transformComp);
					}
					else
					{
						LOGGER(error, fmt::format("[scene] unknown component type: {}", componentName));
					}
				}
			}
		}

		LOGGER(info, fmt::format("[scene] parsed: {}", file));
	}

	void Scene::FindAndAddAsChild(GraphNode& node, uint64_t parentId, seri::IDComponent childIdComponent)
	{
		if (parentId == 0 || node.idComponent.id == parentId)
		{
			GraphNode childNode{};
			childNode.idComponent = childIdComponent;
			node.children.push_back(childNode);
			return;
		}

		for (auto& childNode : node.children)
		{
			FindAndAddAsChild(childNode, parentId, childIdComponent);
		}
	}

}
