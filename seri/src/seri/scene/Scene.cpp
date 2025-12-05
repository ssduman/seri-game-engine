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
	void Scene::Serialize(const std::string& file)
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();

		std::vector<uint64_t> ids{};
		GetAllIDs(_sceneGraphRoot, ids);

		YAML::Node root;

		YAML::Node sceneNode;
		sceneNode["IDComponent"] = seri::IDComponent::Serialize(_idComponent);
		sceneNode["SceneComponent"] = seri::SceneComponent::Serialize(_sceneComponent);

		root["Scene"] = sceneNode;

		YAML::Node entitiesNode;
		for (uint64_t id : ids)
		{
			entt::entity entity = GetEntityById(id);

			YAML::Node entityDataNode;

			if (auto* idComponent = registry.try_get<seri::IDComponent>(entity))
			{
				entityDataNode["IDComponent"] = seri::IDComponent::Serialize(*idComponent);
			}

			if (auto* transformComponent = registry.try_get<seri::TransformComponent>(entity))
			{
				entityDataNode["TransformComponent"] = seri::TransformComponent::Serialize(*transformComponent);
			}

			YAML::Node entityNode;
			entityNode["Entity"] = entityDataNode;

			entitiesNode.push_back(entityNode);
		}
		sceneNode["Entities"] = entitiesNode;

		std::ofstream fout(file);
		fout << root;

		LOGGER(info, fmt::format("[scene] serialized to {}", file));
	}

	void Scene::Deserialize(const std::string& file)
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
		_sceneGraphRoot = GraphNode{};

		auto& registry = seri::scene::SceneManager::GetRegistry();

		if (YAML::Node entities = sceneNode["Entities"])
		{
			for (auto entityItem : entities)
			{
				YAML::Node entityData = entityItem["Entity"];

				entt::entity entity = seri::scene::SceneManager::CreateEntity();

				seri::IDComponent idComp = seri::IDComponent::Deserialize(entityData["IDComponent"]);
				registry.emplace<seri::IDComponent>(entity, idComp);

				FindAndAddAsChild(idComp.parentId, idComp);

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

	void Scene::GetAllIDs(GraphNode& node, std::vector<uint64_t>& ids)
	{
		if (node.idComponent.id != 0)
		{
			ids.push_back(node.idComponent.id);
		}

		for (auto& childNode : node.children)
		{
			GetAllIDs(childNode, ids);
		}
	}

	void Scene::FindAndDelete(uint64_t id)
	{
		FindAndDelete(_sceneGraphRoot, id);
	}

	void Scene::FindAndDelete(GraphNode& node, uint64_t id)
	{
		int deleteIndex = -1;
		for (size_t i = 0; i < node.children.size(); i++)
		{
			if (node.children[i].idComponent.id == id)
			{
				deleteIndex = i;
				break;
			}

			FindAndDelete(node.children[i], id);
		}

		if (deleteIndex != -1)
		{
			node.children.erase(node.children.begin() + deleteIndex);
		}
	}

	void Scene::FindAndAddAsChild(uint64_t parentId, seri::IDComponent childIdComponent)
	{
		FindAndAddAsChild(_sceneGraphRoot, parentId, childIdComponent);
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
