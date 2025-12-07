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
	void Scene::Init()
	{
		_skybox = std::make_shared<seri::Skybox>();
	}

	void Scene::Update()
	{
		_skybox->Update();
	}

	void Scene::Save()
	{
		Serialize(_filePath);
	}

	void Scene::Serialize(const std::string& file)
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();

		std::vector<uint64_t> ids{};
		GetAllEntityIDs(ids);

		YAML::Node root;

		YAML::Node sceneNode;
		sceneNode["IDComponent"] = seri::component::IDComponent::Serialize(_idComponent);
		sceneNode["SceneComponent"] = seri::component::SceneComponent::Serialize(_sceneComponent);

		root["Scene"] = sceneNode;

		YAML::Node entitiesNode;
		for (uint64_t id : ids)
		{
			entt::entity entity = GetEntityByID(id);

			YAML::Node entityDataNode;

			if (auto* idComponent = registry.try_get<seri::component::IDComponent>(entity))
			{
				entityDataNode["IDComponent"] = seri::component::IDComponent::Serialize(*idComponent);
			}

			if (auto* transformComponent = registry.try_get<seri::component::TransformComponent>(entity))
			{
				entityDataNode["TransformComponent"] = seri::component::TransformComponent::Serialize(*transformComponent);
			}

			YAML::Node entityNode;
			entityNode["Entity"] = entityDataNode;

			entitiesNode.push_back(entityNode);
		}
		sceneNode["Entities"] = entitiesNode;

		std::ofstream fout(file);
		fout << root;

		_isDirty = false;

		LOGGER(info, fmt::format("[scene] serialized to {}", file));
	}

	void Scene::Deserialize(const std::string& file)
	{
		_filePath = std::string{ file.c_str() };

		YAML::Node root = YAML::LoadFile(file);

		if (!root["Scene"] || !root["Scene"].IsMap())
		{
			LOGGER(error, fmt::format("[scene] failed to deserialize scene from file: {}", file));
			return;
		}

		YAML::Node sceneNode = root["Scene"];

		_idComponent = seri::component::IDComponent::Deserialize(sceneNode["IDComponent"]);
		_sceneComponent = seri::component::SceneComponent::Deserialize(sceneNode["SceneComponent"]);

		_entityMap.clear();
		_sceneTreeRoot = SceneTreeNode{};

		auto& registry = seri::scene::SceneManager::GetRegistry();

		if (YAML::Node entities = sceneNode["Entities"])
		{
			for (auto entityItem : entities)
			{
				YAML::Node entityData = entityItem["Entity"];

				seri::component::IDComponent idComp = seri::component::IDComponent::Deserialize(entityData["IDComponent"]);

				AddEntityAsChild(idComp.id, idComp.parentId, idComp.name);

				entt::entity entity = GetEntityByID(idComp.id);

				for (auto componentNode : entityData)
				{
					std::string componentName = componentNode.first.as<std::string>();
					YAML::Node componentData = componentNode.second;

					if (componentName == "IDComponent")
					{
					}
					else if (componentName == "TransformComponent")
					{
						seri::component::TransformComponent transformComp = seri::component::TransformComponent::Deserialize(componentData);
						registry.emplace_or_replace<seri::component::TransformComponent>(entity, transformComp);
					}
					else
					{
						LOGGER(error, fmt::format("[scene] unknown component type: {}", componentName));
					}
				}
			}
		}

		_isDirty = false;

		LOGGER(info, fmt::format("[scene] parsed: {}", file));
	}

	void Scene::GetAllEntityIDs(std::vector<uint64_t>& ids)
	{
		GetAllEntityIDs(_sceneTreeRoot, ids);
	}

	void Scene::GetAllEntityIDs(SceneTreeNode& node, std::vector<uint64_t>& ids)
	{
		if (node.id != 0)
		{
			ids.push_back(node.id);
		}

		for (auto& childNode : node.children)
		{
			GetAllEntityIDs(childNode, ids);
		}
	}

	void Scene::DeleteEntity(uint64_t id)
	{
		DeleteEntity(_sceneTreeRoot, id);
	}

	void Scene::DeleteEntity(SceneTreeNode& node, uint64_t id)
	{
		size_t deleteIndex = -1;
		for (size_t i = 0; i < node.children.size(); i++)
		{
			if (node.children[i].id == id)
			{
				deleteIndex = i;
				break;
			}

			DeleteEntity(node.children[i], id);
		}

		if (deleteIndex != -1)
		{
			SetAsDirty();

			entt::entity entity = GetEntityByID(id);
			seri::scene::SceneManager::DestroyEntity(entity);

			_entityMap.erase(id);

			node.children.erase(node.children.begin() + deleteIndex);
		}
	}

	void Scene::AddEntityAsChild(uint64_t id, uint64_t parentId, const std::string& name)
	{
		AddEntityAsChild(_sceneTreeRoot, id, parentId, name);
	}

	void Scene::AddEntityAsChild(SceneTreeNode& node, uint64_t id, uint64_t parentId, const std::string& name)
	{
		if (parentId == 0 || node.id == parentId)
		{
			SetAsDirty();

			entt::entity entity = seri::scene::SceneManager::CreateEntity();

			seri::component::IDComponent idComp = { id, parentId, std::string{ name } };
			seri::scene::SceneManager::GetRegistry().emplace_or_replace<seri::component::IDComponent>(entity, idComp);

			seri::component::TransformComponent transformComp = {};
			seri::scene::SceneManager::GetRegistry().emplace_or_replace<seri::component::TransformComponent>(entity, transformComp);

			SceneTreeNode childNode{};
			childNode.id = id;
			childNode.parentId = parentId;
			node.children.push_back(childNode);

			_entityMap[childNode.id] = entity;

			return;
		}

		for (auto& childNode : node.children)
		{
			AddEntityAsChild(childNode, id, parentId, name);
		}
	}

}
