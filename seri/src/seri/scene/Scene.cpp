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
		_infiniteGrid = std::make_shared<seri::InfiniteGrid>();
	}

	void Scene::Update()
	{
		_skybox->Update();
		_infiniteGrid->Update();
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

		YAML::Node entitiesNode;
		for (uint64_t id : ids)
		{
			entt::entity entity = GetEntityByID(id);

			YAML::Node entityDataNode;

			if (auto* idComp = registry.try_get<seri::component::IDComponent>(entity))
			{
				entityDataNode["IDComponent"] = seri::component::IDComponent::Serialize(*idComp);
			}

			if (auto* transformComp = registry.try_get<seri::component::TransformComponent>(entity))
			{
				entityDataNode["TransformComponent"] = seri::component::TransformComponent::Serialize(*transformComp);
			}

			if (auto* meshComp = registry.try_get<seri::component::MeshComponent>(entity))
			{
				entityDataNode["MeshComponent"] = seri::component::MeshComponent::Serialize(*meshComp);
			}

			if (auto* meshRendererComp = registry.try_get<seri::component::MeshRendererComponent>(entity))
			{
				entityDataNode["MeshRendererComponent"] = seri::component::MeshRendererComponent::Serialize(*meshRendererComp);
			}

			YAML::Node entityNode;
			entityNode["Entity"] = entityDataNode;

			entitiesNode.push_back(entityNode);
		}

		YAML::Node root;
		root["IDComponent"] = seri::component::IDComponent::Serialize(_idComponent);
		root["SceneComponent"] = seri::component::SceneComponent::Serialize(_sceneComponent);
		root["Entities"] = entitiesNode;

		std::ofstream fout(file);
		fout << root;
		fout.flush();

		_isDirty = false;

		LOGGER(info, fmt::format("[scene] serialized to {}", file));
	}

	void Scene::Deserialize(const std::string& file)
	{
		_filePath = std::string{ file.c_str() };

		YAML::Node root = YAML::LoadFile(file);

		if (!root["SceneComponent"] || !root["SceneComponent"].IsMap())
		{
			LOGGER(error, fmt::format("[scene] failed to deserialize scene from file: {}", file));
			return;
		}

		_idComponent = seri::component::IDComponent::Deserialize(root["IDComponent"]);
		_sceneComponent = seri::component::SceneComponent::Deserialize(root["SceneComponent"]);

		_entityMap.clear();
		_sceneTreeRoot = SceneTreeNode{};

		auto& registry = seri::scene::SceneManager::GetRegistry();

		if (YAML::Node entities = root["Entities"])
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
						auto transformComp = seri::component::TransformComponent::Deserialize(componentData);
						registry.emplace_or_replace<seri::component::TransformComponent>(entity, transformComp);
					}
					else if (componentName == "MeshComponent")
					{
						auto meshComp = seri::component::MeshComponent::Deserialize(componentData);
						registry.emplace_or_replace<seri::component::MeshComponent>(entity, meshComp);
					}
					else if (componentName == "MeshRendererComponent")
					{
						auto meshRendererComp = seri::component::MeshRendererComponent::Deserialize(componentData);
						registry.emplace_or_replace<seri::component::MeshRendererComponent>(entity, meshRendererComp);
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

			entt::registry& registry = seri::scene::SceneManager::GetRegistry();

			entt::entity entity = seri::scene::SceneManager::CreateEntity();

			seri::component::IDComponent idComp = { id, parentId, std::string{ name } };
			registry.emplace_or_replace<seri::component::IDComponent>(entity, idComp);

			seri::component::TransformComponent transformComp = {};
			registry.emplace_or_replace<seri::component::TransformComponent>(entity, transformComp);

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

	void Scene::AddComponentToEntity(uint64_t id, seri::component::MeshComponent comp)
	{
		SetAsDirty();

		entt::registry& registry = seri::scene::SceneManager::GetRegistry();

		entt::entity entity = GetEntityByID(id);

		registry.emplace_or_replace<seri::component::MeshComponent>(entity, comp);
	}

	void Scene::AddComponentToEntity(uint64_t id, seri::component::MeshRendererComponent comp)
	{
		SetAsDirty();

		entt::registry& registry = seri::scene::SceneManager::GetRegistry();

		entt::entity entity = GetEntityByID(id);

		registry.emplace_or_replace<seri::component::MeshRendererComponent>(entity, comp);
	}

}
